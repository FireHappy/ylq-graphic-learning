#include "Triangle.hpp"
#include "rasterizer.hpp"
#include <eigen3/Eigen/Eigen>
#include <iostream>
#include <opencv2/opencv.hpp>

constexpr double MY_PI = 3.1415926;

Eigen::Matrix4f get_view_matrix(Eigen::Vector3f eye_pos)
{
    Eigen::Matrix4f view = Eigen::Matrix4f::Identity();

    Eigen::Matrix4f translate;
    translate << 1, 0, 0, -eye_pos[0],
        0, 1, 0, -eye_pos[1],
        0, 0, 1, -eye_pos[2],
        0, 0, 0, 1;

    // 先写出一个从标准坐标系到人眼坐标的旋转矩阵
    Eigen::Matrix4f rotate;
    rotate << 1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, -1, 0,
        0, 0, 0, 1;

    // 因为旋转矩阵是单位矩阵,因此矩阵的逆=矩阵的倒置
    view = rotate.transpose() * translate * view;

    return view;
}

float anlge_to_radian(float angle)
{
    return angle * MY_PI / 180;
}

/**
 * @brief Get the angle object
 *  计算两个向量之间的夹角(弧度)
 * @param v1
 * @param v2
 * @return float
 */
float get_angle(Eigen::Vector3f v1, Eigen::Vector3f v2)
{
    float dot = v1.dot(v2);
    double cosθ = dot / (v1.norm() * v2.norm());
    return std::acos(cosθ);
}

Eigen::Matrix4f get_rotation(Eigen::Vector3f axis, float anlge)
{
    // 1.获取轴在ZY平面的投影
    Vector3f axis_zy = Eigen::Vector3f(0, axis.y(), axis.z());
    Eigen::Matrix4f rotate_x;
    if (axis_zy.x() == 0 && axis_zy.y() == 0 && axis_zy.z() == 0)
    {
        rotate_x = Eigen::Matrix4f::Identity();
    }
    else
    {
        // 2.计算ZYaxis与Y轴的夹角
        float anlge_zy = get_angle(axis_zy, Eigen::Vector3f(0, 1, 0));
        // 3.判断ZYaxis是在Y轴的左边还是右边
        if (axis_zy.z() > 0)
        {
            anlge_zy = -anlge_zy;
        }
        // 4.构建让ZYaxis转向Y轴的旋转矩阵,等同与ZYaxis上的某个点,绕X轴旋转anlge到Y轴,角的正负由3判断
        rotate_x << 1, 0, 0, 0,
            0, cos(anlge_zy), -sin(anlge_zy), 0,
            0, sin(anlge_zy), cos(anlge_zy), 0,
            0, 0, 0, 1;
    }

    // 1.获取轴在XY平面的投影
    Vector3f axis_xy = Eigen::Vector3f(axis.x(), axis.y(), 0);
    Eigen::Matrix4f rotate_z;
    if (axis_xy.x() == 0 && axis_xy.x() == 0 && axis_xy.x() == 0)
    {
        rotate_z = Eigen::Matrix4f::Identity();
    }
    else
    {
        // 2.计算XYaxis与Y轴的夹角
        float anlge_xy = get_angle(axis_xy, Eigen::Vector3f(0, 1, 0));
        // 3.判断ZYaxis是在Y轴的左边还是右边
        if (axis_xy.x() < 0)
        {
            anlge_xy = -anlge_xy;
        }
        // 4.构建让axis_xy转向Y轴的旋转矩阵,等同与axis_xy上的某个点,绕Z轴旋转anlge到Y轴,角的正负由3判断
        rotate_z << cos(anlge_xy), -sin(anlge_xy), 0, 0,
            cos(anlge_xy), sin(anlge_xy), 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1;
    }

    // 综上我们就得到了将axis转换到Y轴上的方法
    Eigen::Matrix4f rotate;
    rotate = rotate_z * rotate_x;
    // 构建绕Y轴旋转angle矩阵
    Eigen::Matrix4f rotate_y;
    rotate_y << cos(anlge_to_radian(anlge)), 0, sin(anlge_to_radian(anlge)), 0,
        0, 1, 0, 0,
        -sin(anlge_to_radian(anlge)), 0, cos(anlge_to_radian(anlge)), 0,
        0, 0, 0, 1;
    // 基于 P'= My->a *My-angle *Ma->y*P
    return rotate.transpose() * rotate_y * rotate;
}

Eigen::Matrix4f get_model_matrix(float rotation_angle)
{
    Eigen::Matrix4f model = Eigen::Matrix4f::Identity();

    // TODO: Implement this function
    // Create the model matrix for rotating the triangle around the Z a'aaxis.
    // Then return it.
    Eigen::Matrix4f rotate;
    rotate = get_rotation(Eigen::Vector3f(0, 0, 1), rotation_angle);

    // rotate << cos(anlge_to_radian(rotation_angle)), -sin(anlge_to_radian(rotation_angle)), 0, 0,
    //     sin(anlge_to_radian(rotation_angle)), cos(anlge_to_radian(rotation_angle)), 0, 0,
    //     0, 0, 1, 0,
    //     0, 0, 0, 1;
    model = rotate * model;

    return model;
}

Eigen::Matrix4f get_projection_matrix(float eye_fov, float aspect_ratio,
                                      float zNear, float zFar)
{
    // Students will implement this function

    Eigen::Matrix4f projection = Eigen::Matrix4f::Identity();

    // TODO: Implement this function
    // Create the projection matrix for the given parameters.
    // Then return it.

    // 构建正交投影矩阵
    Eigen::Matrix4f Mortho = Eigen::Matrix4f::Identity();
    float n = zNear, f = zFar;
    float t = tan(anlge_to_radian(eye_fov / 2)) * n;
    float b = -t;
    float r = aspect_ratio * t;
    float l = -r;

    // 将bounds长方体,移动到原点
    Eigen::Matrix4f translate = Eigen::Matrix4f::Identity();
    translate << 1, 0, 0, -(r + l) / 2,
        0, 1, 0, -(t + b) / 2,
        0, 0, 1, -(n + f) / 2,
        0, 0, 0, 1;

    // 将bounds长方体,缩放成边长为2的立方体
    Eigen::Matrix4f scale = Eigen::Matrix4f::Identity();
    translate << 2 / (r - l), 0, 0, 0,
        0, 2 / (t - b), 0, 0,
        0, 0, 2 / (n - f), 0,
        0, 0, 0, 1;

    Mortho = scale * translate;

    // 构建将视椎体压缩成长方体的矩阵
    Eigen::Matrix4f Mpersp_ortho;
    Mpersp_ortho << n, 0, 0, 0,
        0, n, 0, 0,
        0, 0, n + f, -n * f,
        0, 0, 1, 0;

    projection = Mortho * Mpersp_ortho * projection;
    return projection;
}

int main(int argc, const char **argv)
{
    std::cout << "build success !!!" << std::endl;
    float angle = 0;
    bool command_line = false;
    std::string filename = "output.png";

    if (argc >= 3)
    {
        command_line = true;
        angle = std::stof(argv[2]); // -r by default
        if (argc == 4)
        {
            filename = std::string(argv[3]);
        }
        else
            return 0;
    }

    rst::rasterizer r(700, 700);

    Eigen::Vector3f eye_pos = {0, 0, 5};

    std::vector<Eigen::Vector3f> pos{{2, 0, -2}, {0, 2, -2}, {-2, 0, -2}};

    // 点的索引
    std::vector<Eigen::Vector3i> ind{{0, 1, 2}};

    auto pos_id = r.load_positions(pos);
    auto ind_id = r.load_indices(ind);

    int key = 0;
    int frame_count = 0;

    if (command_line)
    {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        r.set_model(get_model_matrix(angle));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45, 1, 0.1, 50));

        r.draw(pos_id, ind_id, rst::Primitive::Triangle);
        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);

        cv::imwrite(filename, image);

        return 0;
    }

    while (key != 27)
    {
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

        if (key == 'a')
        {
            angle += 10;
        }
        else if (key == 'd')
        {
            angle -= 10;
        }
    }

    return 0;
}
