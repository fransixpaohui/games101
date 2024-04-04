#include "Triangle.hpp"
#include "rasterizer.hpp"
#include <eigen3/Eigen/Eigen>
#include <iostream>
#include <opencv2/opencv.hpp>

constexpr double MY_PI = 3.1415926;

// model transformation 将物体摆正所需角度所需要的变换
Eigen::Matrix4f get_model_matrix(float rotation_angle)
{
    Eigen::Matrix4f model = Eigen::Matrix4f::Identity();
    //根据Rodrigue旋转公式得到旋转矩阵
    model <<
        cos((rotation_angle * MY_PI) / 180), -sin((rotation_angle * MY_PI) / 180), 0, 0,
        sin((rotation_angle * MY_PI) / 180), cos((rotation_angle * MY_PI) / 180), 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1;

    return model;
}

// view transformation 将视点移到原点所需要的变换
Eigen::Matrix4f get_view_matrix(Eigen::Vector3f eye_pos)
{
    Eigen::Matrix4f view = Eigen::Matrix4f::Identity();

    Eigen::Matrix4f translate;
    translate <<
        1, 0, 0, -eye_pos[0],
        0, 1, 0, -eye_pos[1],
        0, 0, 1, -eye_pos[2],
        0, 0, 0, 1;

    view = translate * view;

    return view;
}

// projection transformation 将3D向视点角度投影到固定视平面的变换
Eigen::Matrix4f get_projection_matrix(float eye_fov, float aspect_ratio,
    float zNear, float zFar)
{
    Eigen::Matrix4f projection = Eigen::Matrix4f::Identity();

    Eigen::Matrix4f persp2ortho = Eigen::Matrix4f::Identity();
    // 透视投影第一步，将图像挤压到视平面上，(视平面即相机在某点看图像时因透视而产生的图像所在的平面)
    //“squish” the frustum into a cuboid (n->n,f->f)
    persp2ortho <<
        zNear, 0, 0, 0,
        0, zNear, 0, 0,
        0, 0, zNear + zFar, -(zNear * zFar),
        0, 0, 1, 0;

    // 第二步，正交投影
    Eigen::Matrix4f ortho = Eigen::Matrix4f::Identity();
    Eigen::Matrix4f scale = Eigen::Matrix4f::Identity();
    Eigen::Matrix4f translation = Eigen::Matrix4f::Identity();

    // t means top, b means bottom,l means left,r means right.
    float t = tanf(eye_fov / 2) * abs(zNear);
    float b = -t;
    float r = aspect_ratio * t;
    float l = -r;

    // scale矩阵，将原图形缩放为标准长度为2的立方体
    scale <<
        2 / (r - l), 0, 0, 0,
        0, 2 / (t - b), 0, 0,
        0, 0, 2 / (zNear - zFar), 0,
        0, 0, 0, 1;

    // 将立方体的中心移至源点
    translation <<
        1, 0, 0, -((r + l) / 2),
        0, 1, 0, -((t + b) / 2),
        0, 0, 1, -((zNear + zFar) / 2),
        0, 0, 0, 1;

    ortho = scale * translation;
    projection = ortho * persp2ortho;

    return projection;
}

int main(int argc, const char** argv)
{
    float angle = 0;
    bool command_line = false;
    std::string filename = "output.png";

    if (argc >= 3) {
        command_line = true;
        angle = std::stof(argv[2]); // -r by default
        if (argc == 4) {
            filename = std::string(argv[3]);
        }
    }

    rst::rasterizer r(700, 700);

    Eigen::Vector3f eye_pos = { 0, 0, 5 };

    std::vector<Eigen::Vector3f> pos{ {2, 0, -2}, {0, 2, -2}, {-2, 0, -2} };

    std::vector<Eigen::Vector3i> ind{ {0, 1, 2} };

    auto pos_id = r.load_positions(pos);
    auto ind_id = r.load_indices(ind);

    int key = 0;
    int frame_count = 0;

    if (command_line) {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);
        // 第一步，model transformation 将物体摆到需要的角度
        r.set_model(get_model_matrix(angle));
        // 第二步，view transformation 将物体移至原点
        r.set_view(get_view_matrix(eye_pos));
        // 第三步，projection transformation
        r.set_projection(get_projection_matrix(45, 1, 0.1, 50));

        r.draw(pos_id, ind_id, rst::Primitive::Triangle);
        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);

        cv::imwrite(filename, image);

        return 0;
    }
    //键盘输入不是ESC
    while (key != 27) {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        r.set_model(get_model_matrix(angle));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45, 1, 0.1, 50));

        r.draw(pos_id, ind_id, rst::Primitive::Triangle);

        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);
        cv::imshow("image", image);
        key = cv::waitKey(10);

        std::cout << "frame count: " << frame_count++ << '\n';

        if (key == 'a') {
            angle += 10;
        }
        else if (key == 'd') {
            angle -= 10;
        }
    }

    return 0;
}
