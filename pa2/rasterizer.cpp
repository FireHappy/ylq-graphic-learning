//
// Created by goksu on 4/6/19.
//

#include <algorithm>
#include <iostream>
#include <vector>
#include "rasterizer.hpp"
#include <opencv2/opencv.hpp>
#include <math.h>

rst::pos_buf_id rst::rasterizer::load_positions(const std::vector<Eigen::Vector3f> &positions)
{
    auto id = get_next_id();
    pos_buf.emplace(id, positions);

    return {id};
}

rst::ind_buf_id rst::rasterizer::load_indices(const std::vector<Eigen::Vector3i> &indices)
{
    auto id = get_next_id();
    ind_buf.emplace(id, indices);

    return {id};
}

rst::col_buf_id rst::rasterizer::load_colors(const std::vector<Eigen::Vector3f> &cols)
{
    auto id = get_next_id();
    col_buf.emplace(id, cols);

    return {id};
}

Vector4f to_vec4(const Eigen::Vector3f &v3, float w = 1.0f)
{
    return Vector4f(v3.x(), v3.y(), v3.z(), w);
}

float cross(Vector2f v1, Vector2f v2)
{
    return v1.x() * v2.y() - v1.y() * v2.x();
}

static bool insideTriangle(int x, int y, const Vector3f *_v)
{
    // 二维屏幕空间
    Vector2f p = Vector2f(x, y);
    Vector2f a = Vector2f(_v[0].x(), _v[0].y());
    Vector2f b = Vector2f(_v[1].x(), _v[1].y());
    Vector2f c = Vector2f(_v[2].x(), _v[2].y());
    Vector2f ab = b - a, ap = p - a;
    Vector2f bc = c - b, bp = p - b;
    Vector2f ca = a - c, cp = p - c;
    float r1 = cross(ab, ap);
    float r2 = cross(bc, bp);
    float r3 = cross(ca, cp);

    int overZeroCount = 0;
    if (r1 > 0)
        overZeroCount++;
    if (r2 > 0)
        overZeroCount++;
    if (r3 > 0)
        overZeroCount++;

    // z的符号相同,则证明的点在三角形内,否则在三角形外
    return overZeroCount == 0 || overZeroCount == 3;
}

static std::tuple<float, float, float> computeBarycentric2D(float x, float y, const Vector3f *v)
{
    float c1 = (x * (v[1].y() - v[2].y()) + (v[2].x() - v[1].x()) * y + v[1].x() * v[2].y() - v[2].x() * v[1].y()) / (v[0].x() * (v[1].y() - v[2].y()) + (v[2].x() - v[1].x()) * v[0].y() + v[1].x() * v[2].y() - v[2].x() * v[1].y());
    float c2 = (x * (v[2].y() - v[0].y()) + (v[0].x() - v[2].x()) * y + v[2].x() * v[0].y() - v[0].x() * v[2].y()) / (v[1].x() * (v[2].y() - v[0].y()) + (v[0].x() - v[2].x()) * v[1].y() + v[2].x() * v[0].y() - v[0].x() * v[2].y());
    float c3 = (x * (v[0].y() - v[1].y()) + (v[1].x() - v[0].x()) * y + v[0].x() * v[1].y() - v[1].x() * v[0].y()) / (v[2].x() * (v[0].y() - v[1].y()) + (v[1].x() - v[0].x()) * v[2].y() + v[0].x() * v[1].y() - v[1].x() * v[0].y());
    return {c1, c2, c3};
}

void rst::rasterizer::draw(pos_buf_id pos_buffer, ind_buf_id ind_buffer, col_buf_id col_buffer, Primitive type)
{
    auto &buf = pos_buf[pos_buffer.pos_id];
    auto &ind = ind_buf[ind_buffer.ind_id];
    auto &col = col_buf[col_buffer.col_id];

    float f1 = (50 - 0.1) / 2.0;
    float f2 = (50 + 0.1) / 2.0;

    Eigen::Matrix4f mvp = projection * view * model;
    for (auto &i : ind)
    {
        Triangle t;
        Eigen::Vector4f v[] = {
            mvp * to_vec4(buf[i[0]], 1.0f),
            mvp * to_vec4(buf[i[1]], 1.0f),
            mvp * to_vec4(buf[i[2]], 1.0f)};
        // Homogeneous division
        for (auto &vec : v)
        {
            vec /= vec.w();
        }
        // Viewport transformation
        for (auto &vert : v)
        {
            vert.x() = 0.5 * width * (vert.x() + 1.0);
            vert.y() = 0.5 * height * (vert.y() + 1.0);
            vert.z() = vert.z() * f1 + f2;
        }

        for (int i = 0; i < 3; ++i)
        {
            t.setVertex(i, v[i].head<3>());
            t.setVertex(i, v[i].head<3>());
            t.setVertex(i, v[i].head<3>());
        }

        auto col_x = col[i[0]];
        auto col_y = col[i[1]];
        auto col_z = col[i[2]];

        t.setColor(0, col_x[0], col_x[1], col_x[2]);
        t.setColor(1, col_y[0], col_y[1], col_y[2]);
        t.setColor(2, col_z[0], col_z[1], col_z[2]);
        rasterize_triangle(t);
    }
}

// Screen space rasterization
void rst::rasterizer::rasterize_triangle(const Triangle &t)
{
    auto v = t.toVector4();

    // Implement this function to check if the point(x, y) is inside the triangle represented by _v[0], _v[1], _v[2]
    int minX = v[0].x();
    int maxX = v[0].x();
    int minY = v[0].y();
    int maxY = v[0].y();

    std::array<Vector3f, 3> _v;

    for (size_t i = 0; i < v.size(); i++)
    {
        if (minX > v[i].x())
            minX = v[i].x();
        if (maxX < v[i].x())
            maxX = v[i].x() + 1;

        if (minY > v[i].y())
            minY = v[i].y();
        if (maxY < v[i].y())
            maxY = v[i].y() + 1;

        _v[i] = Vector3f(v[i].x(), v[i].y(), v[i].z());
    }

    // iterate through the pixel and find if the current pixel is inside the triangle
    for (size_t i = minX; i < maxX; i++)
    {
        for (size_t j = minY; j < maxY; j++)
        {
            if (insideTriangle(i, j, _v.begin()))
            {
                std::cout << "inside triangle" << '\n';
                Vector3f point = Vector3f(i, j, 1.0f);
                set_pixel(point, *t.color);
            }
            else
            {
                std::cout << "outside triangle" << '\n';
            }
        }
    }

    // If so, use the following code to get the interpolated z value.
    // auto[alpha, beta, gamma] = computeBarycentric2D(x, y, t.v);
    // float w_reciprocal = 1.0/(alpha / v[0].w() + beta / v[1].w() + gamma / v[2].w());
    // float z_interpolated = alpha * v[0].z() / v[0].w() + beta * v[1].z() / v[1].w() + gamma * v[2].z() / v[2].w();
    // z_interpolated *= w_reciprocal;

    // TODO : set the current pixel (use the set_pixel function) to the color of the triangle (use getColor function) if it should be painted.
}

void rst::rasterizer::set_model(const Eigen::Matrix4f &m)
{
    model = m;
}

void rst::rasterizer::set_view(const Eigen::Matrix4f &v)
{
    view = v;
}

void rst::rasterizer::set_projection(const Eigen::Matrix4f &p)
{
    projection = p;
}

void rst::rasterizer::clear(rst::Buffers buff)
{
    if ((buff & rst::Buffers::Color) == rst::Buffers::Color)
    {
        std::fill(frame_buf.begin(), frame_buf.end(), Eigen::Vector3f{0, 0, 0});
    }
    if ((buff & rst::Buffers::Depth) == rst::Buffers::Depth)
    {
        std::fill(depth_buf.begin(), depth_buf.end(), std::numeric_limits<float>::infinity());
    }
}

rst::rasterizer::rasterizer(int w, int h) : width(w), height(h)
{
    frame_buf.resize(w * h);
    depth_buf.resize(w * h);
}

int rst::rasterizer::get_index(int x, int y)
{
    return (height - 1 - y) * width + x;
}

void rst::rasterizer::set_pixel(const Eigen::Vector3f &point, const Eigen::Vector3f &color)
{
    // old index: auto ind = point.y() + point.x() * width;
    auto ind = (height - 1 - point.y()) * width + point.x();
    frame_buf[ind] = color;
}