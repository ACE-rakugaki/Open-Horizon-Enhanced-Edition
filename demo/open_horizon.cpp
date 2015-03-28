//
// open horizon -- undefined_darkness@outlook.com
//

#include "GLFW/glfw3.h"

#include "qdf_provider.h"
#include "../dpl.h" //ToDo: dpl_provider.h

#include "aircraft.h"
#include "location.h"
#include "location_params.h"
#include "clouds.h"
#include "render/screen_quad.h"

#include "render/vbo.h"
#include "render/fbo.h"
#include "render/render.h"
#include "math/vector.h"
#include "math/quaternion.h"
#include "scene/camera.h"
#include "scene/shader.h"
#include "system/system.h"
#include "shared.h"
#include "resources/file_resources_provider.h"

#include "scene/postprocess.h"

#include <math.h>
#include <vector>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>

//------------------------------------------------------------

class plane_camera
{
public:
    void add_delta_rot(float dx, float dy);
    void reset_delta_rot() { m_drot.x = 0.0f; m_drot.y = 3.14f; }
    void add_delta_pos(float dx, float dy, float dz);

    void set_aspect(float aspect);
    void set_pos(const nya_math::vec3 &pos) { m_pos = pos; update(); }
    void set_rot(const nya_math::quat &rot) { m_rot = rot; update(); }

private:
    void update();

public:
    plane_camera(): m_dpos(0.0f, 3.0f, 12.0f) { m_drot.y = 3.14f; }

private:
    nya_math::vec3 m_drot;
    nya_math::vec3 m_dpos;

    nya_math::vec3 m_pos;
    nya_math::quat m_rot;
};

//------------------------------------------------------------

void plane_camera::add_delta_rot(float dx, float dy)
{
    m_drot.x += dx;
    m_drot.y += dy;
    
    const float max_angle = 3.14f * 2.0f;
    
    if (m_drot.x > max_angle)
        m_drot.x -= max_angle;
    
    if (m_drot.x < -max_angle)
        m_drot.x += max_angle;
    
    if (m_drot.y > max_angle)
        m_drot.y -= max_angle;
    
    if (m_drot.y< -max_angle)
        m_drot.y+=max_angle;
    
    update();
}

//------------------------------------------------------------

void plane_camera::add_delta_pos(float dx, float dy, float dz)
{
    m_dpos.x -= dx;
    m_dpos.y -= dy;
    m_dpos.z -= dz;
    if (m_dpos.z < 5.0f)
        m_dpos.z = 5.0f;

    if (m_dpos.z > 20000.0f)
        m_dpos.z = 20000.0f;

    update();
}

//------------------------------------------------------------

void plane_camera::set_aspect(float aspect)
{
    nya_scene::get_camera().set_proj(55.0, aspect, 1.0, 64000.0);
                      //1300.0);
    update();
}

//------------------------------------------------------------

void plane_camera::update()
{
    nya_math::quat r = m_rot;
    r.v.x = -r.v.x, r.v.y = -r.v.y;

    r = r*nya_math::quat(m_drot.x, m_drot.y, 0.0f);
    //nya_math::vec3 r=m_drot+m_rot;

    //cam->set_rot(r.y*180.0f/3.14f,r.x*180.0f/3.14f,0.0);
    nya_scene::get_camera().set_rot(r);

    //nya_math::quat rot(-r.x,-r.y,0.0f);
    //rot=rot*m_rot;

    //nya_math::vec3 pos=m_pos+rot.rotate(m_dpos);
    r.v.x = -r.v.x, r.v.y = -r.v.y;
    nya_math::vec3 pos = m_pos + r.rotate(m_dpos);

    nya_scene::get_camera().set_pos(pos.x, pos.y, pos.z);
}

//------------------------------------------------------------

nya_scene::texture load_tonecurve(const char *file_name)
{
    nya_scene::shared_texture t;
    auto res = shared::load_resource(file_name);
    assert(res.get_size() == 256 * 3);

    const unsigned char *data = (const unsigned char *)res.get_data();
    unsigned char color[256 * 3];
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 256; ++j)
            color[j*3+i]=data[j+i*256];
    }

    t.tex.build_texture(color, 256, 1, nya_render::texture::color_rgb);
    t.tex.set_wrap(false, false);
    res.free();

    nya_scene::texture result;
    result.create(t);
    return result;
}

//------------------------------------------------------------

int main(void)
{
    const char *plane_name = "su35"; //f22a su35 b02a pkfa su25 su33 su34  kwmr
    const char *plane_color = "color02"; // = 0;
    const char *location_name = "ms01"; //ms01 ms50 ms10

    //plane_name = "f22a",plane_color=0;

#ifndef _WIN32
    chdir(nya_system::get_app_path());
#endif

    qdf_resources_provider qdfp;
    if (!qdfp.open_archive("datafile.qdf"))
    {
        printf("unable to open datafile.qdf");
        return 0;
    }

    nya_resources::set_resources_provider(&qdfp);

    class target_resource_provider: public nya_resources::resources_provider
    {
        nya_resources::resources_provider &m_provider;

    public:
        target_resource_provider(nya_resources::resources_provider &provider): m_provider(provider) {}

    private:
        nya_resources::resource_data *access(const char *resource_name)
        {
            if (!resource_name)
                return 0;

            const std::string str(resource_name);
            if (m_provider.has(("target/" + str).c_str()))
                return m_provider.access(("target/" + str).c_str());

            if (m_provider.has(("common/" + str).c_str()))
                return m_provider.access(("common/" + str).c_str());

            if (m_provider.has(str.c_str()))
                return m_provider.access(str.c_str());

            static nya_resources::file_resources_provider fprov;
            static bool dont_care = fprov.set_folder(nya_system::get_app_path()); dont_care = dont_care;

            return fprov.access(resource_name);
        }

        bool has(const char *resource_name)
        {
            if (!resource_name)
                return false;

            const std::string str(resource_name);
            return m_provider.has(("target/" + str).c_str()) || m_provider.has(("common/" + str).c_str()) || m_provider.has(resource_name);
        }
    } trp(nya_resources::get_resources_provider());

    nya_resources::set_resources_provider(&trp);

    if (!glfwInit())
        return -1;

    GLFWwindow *window = glfwCreateWindow(1000, 600, "open horizon", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    for (int i = 0; glfwJoystickPresent(i); ++i)
    {
        const char *name = glfwGetJoystickName(i);

        int axis_count = 0, buttons_count = 0;
        glfwGetJoystickAxes(i, &axis_count);
        glfwGetJoystickButtons(i, &buttons_count);
        printf("joy%d: %s %d axis %d buttons\n", i, name, axis_count, buttons_count);
    }

    glfwMakeContextCurrent(window);

    nya_render::texture::set_default_aniso(2);

    class : public nya_scene::postprocess
    {
    public:
        location loc;
        aircraft player_plane;
        effect_clouds clouds;

    private:
        void draw_scene(const char *pass, const char *tags) override
        {
            if (strcmp(tags, "location") == 0)
                loc.draw();
            else if (strcmp(tags, "player") == 0)
                player_plane.draw();
            else if (strcmp(tags, "clouds_flat") == 0)
                clouds.draw_flat();
            else if (strcmp(tags, "clouds_obj") == 0)
                clouds.draw_obj();
        }
    } scene;

    scene.load("postprocess.txt");
    auto curve = load_tonecurve((std::string("Map/tonecurve_") + location_name + ".tcb").c_str());
    scene.set_texture("color_curve", nya_scene::texture_proxy(curve));
    scene.loc.load(location_name);
    scene.clouds.load(location_name);
    scene.player_plane.load(plane_name, plane_color);
    scene.player_plane.set_pos(nya_math::vec3(-300, 50, 2000));

    plane_camera camera;
    //camera.add_delta_rot(0.1f,0.0f);
    if (plane_name[0] == 'b')
        camera.add_delta_pos(0.0f, -2.0f, -20.0f);

    double mx,my;
    glfwGetCursorPos(window, &mx, &my);

    int frame_counter = 0;
    int frame_counter_time = 0;
    int fps = 0;

    int screen_width = 0, screen_height = 0;
    unsigned long app_time = nya_system::get_time();
    while (!glfwWindowShouldClose(window))
    {
        unsigned long time = nya_system::get_time();
        int dt = int(time - app_time);
        if (dt > 1000)
            dt = 1000;

        frame_counter_time += dt;
        ++frame_counter;
        if (frame_counter_time > 1000)
        {
            fps = frame_counter;
            frame_counter = 0;
            frame_counter_time -= 1000;
        }
        app_time = time;

        static bool paused = false;
        static bool speed10x = false;
        if (!paused)
            scene.player_plane.update(speed10x ? dt * 10 : dt);

        char title[255];
        sprintf(title,"speed: %7d alt: %7d \t fps: %3d  %s", int(scene.player_plane.get_speed()), int(scene.player_plane.get_alt()), fps, paused ? "paused" : "");
        glfwSetWindowTitle(window,title);

        int new_screen_width, new_screen_height;
        glfwGetFramebufferSize(window, &new_screen_width, &new_screen_height);
        if (new_screen_width != screen_width || new_screen_height != screen_height)
        {
            screen_width = new_screen_width, screen_height = new_screen_height;
            scene.resize(screen_width, screen_height);
            camera.set_aspect(screen_height > 0 ? float(screen_width) / screen_height : 1.0f);
        }

        camera.set_pos(scene.player_plane.get_pos());
        camera.set_rot(scene.player_plane.get_rot());

        scene.loc.update(dt);
        scene.draw(dt);

        glfwSwapBuffers(window);
        glfwPollEvents();

        double x, y;
        glfwGetCursorPos(window, &x, &y);
        if (glfwGetMouseButton(window, 0))
            camera.add_delta_rot((y - my) * 0.03, (x - mx) * 0.03);
        //else
          //  camera.reset_delta_rot();

        if (glfwGetMouseButton(window, 1))
            camera.add_delta_pos(0, 0, my - y);

        mx = x; my = y;

        nya_math::vec3 c_rot;
        float c_throttle = 0.0f, c_brake = 0.0f;

        int axis_count = 0, buttons_count = 0;
        const float *axis = glfwGetJoystickAxes(0, &axis_count);
        const unsigned char *buttons = glfwGetJoystickButtons(0, &buttons_count);
        const float joy_dead_zone = 0.1f;
        if (axis_count > 1)
        {
            if (fabsf(axis[0]) > joy_dead_zone) c_rot.z = axis[0];
            if (fabsf(axis[1]) > joy_dead_zone) c_rot.x = axis[1];
        }

        if (axis_count > 3)
        {
            if (fabsf(axis[2]) > joy_dead_zone) camera.add_delta_rot(0.0f, -axis[2] * 0.05f);
            if (fabsf(axis[3]) > joy_dead_zone) camera.add_delta_rot(axis[3] * 0.05f, 0.0f);
        }

        if (buttons_count > 11)
        {
            if (buttons[8]) c_rot.y = -1.0f;
            if (buttons[9]) c_rot.y = 1.0f;
            if (buttons[10]) c_brake = 1.0f;
            if (buttons[11]) c_throttle = 1.0f;

            if (buttons[2]) camera.reset_delta_rot();

            static bool last_btn3 = false;
            if (buttons[3] !=0 && !last_btn3)
                paused = !paused;

            last_btn3 = buttons[3] != 0;
        }

        if (glfwGetKey(window, GLFW_KEY_W)) c_throttle = 1.0f;
        if (glfwGetKey(window, GLFW_KEY_S)) c_brake = 1.0f;
        if (glfwGetKey(window, GLFW_KEY_A)) c_rot.y = -1.0f;
        if (glfwGetKey(window, GLFW_KEY_D)) c_rot.y = 1.0f;
        if (glfwGetKey(window, GLFW_KEY_UP)) c_rot.x = 1.0f;
        if (glfwGetKey(window, GLFW_KEY_DOWN)) c_rot.x = -1.0f;
        if (glfwGetKey(window, GLFW_KEY_LEFT)) c_rot.z = -1.0f;
        if (glfwGetKey(window, GLFW_KEY_RIGHT)) c_rot.z = 1.0f;

        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL)) scene.player_plane.fire_mgun();

        static bool last_control_rocket = false, last_control_special = false;

        if (glfwGetKey(window, GLFW_KEY_SPACE))
        {
            if (!last_control_rocket)
                scene.player_plane.fire_rocket();
            last_control_rocket = true;
        }
        else
            last_control_rocket = false;

        if (glfwGetKey(window, GLFW_KEY_Q))
        {
            if (!last_control_special)
                scene.player_plane.change_weapon();
            last_control_special = true;
        }
        else
            last_control_special = false;

        //if (glfwGetKey(window, GLFW_KEY_L)) loc.load(location_name);

        static bool last_btn_p = false;
        if (glfwGetKey(window, GLFW_KEY_P) && !last_btn_p)
            paused = !paused;

        last_btn_p = glfwGetKey(window, GLFW_KEY_P);

        speed10x = glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT);

        scene.player_plane.set_controls(c_rot, c_throttle, c_brake);
    }

    glfwTerminate();

    return 0;
}

//------------------------------------------------------------
