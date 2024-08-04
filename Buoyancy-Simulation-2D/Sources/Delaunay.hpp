#pragma once

#include "Common.cpp"


struct Triangle {
    p p1, p2, p3;
    p circumcenter;
    float radiusSquared;

    Triangle(const p& a, const p& b, const p& c) : p1(a), p2(b), p3(c) {
        calculateCircumcircle();
    }

    void calculateCircumcircle() {
        // Calculate the circumcenter
        float d = 2 * (p1.x * (p2.y - p3.y) + p2.x * (p3.y - p1.y) + p3.x * (p1.y - p2.y));
        float x = ((p1.x * p1.x + p1.y * p1.y) * (p2.y - p3.y) +
            (p2.x * p2.x + p2.y * p2.y) * (p3.y - p1.y) +
            (p3.x * p3.x + p3.y * p3.y) * (p1.y - p2.y)) / d;
        float y = ((p1.x * p1.x + p1.y * p1.y) * (p3.x - p2.x) +
            (p2.x * p2.x + p2.y * p2.y) * (p1.x - p3.x) +
            (p3.x * p3.x + p3.y * p3.y) * (p2.x - p1.x)) / d;
        circumcenter = p(x, y);

        // Calculate the radius squared
        radiusSquared = (circumcenter.x - p1.x) * (circumcenter.x - p1.x) +
            (circumcenter.y - p1.y) * (circumcenter.y - p1.y);
    }

    bool circumcircleContains(const p& point) const {
        float dx = point.x - circumcenter.x;
        float dy = point.y - circumcenter.y;
        return (dx * dx + dy * dy) <= radiusSquared;
    }
};


Triangle createSuperTriangle(const std::vector<p>& points) {
    if (points.empty()) 
    {
        throw std::runtime_error("Point set is empty");
    }

    // Finding the bounding box
    float minX = std::numeric_limits<float>::max();
    float minY = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::lowest();
    float maxY = std::numeric_limits<float>::lowest();

    for (const auto& point : points) {
        minX = std::min(minX, point.x);
        minY = std::min(minY, point.y);
        maxX = std::max(maxX, point.x);
        maxY = std::max(maxY, point.y);
    }

    // Calculate center and dimensions
    float centerX = (minX + maxX) / 2;
    float centerY = (minY + maxY) / 2;
    float width = maxX - minX;
    float height = maxY - minY;

    // Calculate radius of the circumcircle of the bounding box
    float radius = std::sqrt(width * width + height * height) / 2;

    // Add a small buffer for numerical stability
    radius *= 1.01f;

    // Calculate vertices of the equilateral triangle
    const float sqrt3 = std::sqrt(3.0f);
    p v1(centerX - sqrt3 * radius, centerY - radius);
    p v2(centerX + sqrt3 * radius, centerY - radius);
    p v3(centerX, centerY + 2 * radius);

    // Create and return the Triangle object
    return Triangle(v1, v2, v3);
}






struct Edge {
    p p1, p2;
    Edge(const p& a, const p& b) : p1(a), p2(b) {}
    bool operator==(const Edge& other) const {
        return (p1.x == other.p1.x && p1.y == other.p1.y && p2.x == other.p2.x && p2.y == other.p2.y) ||
            (p1.x == other.p2.x && p1.y == other.p2.y && p2.x == other.p1.x && p2.y == other.p1.y);
    }
};

std::vector<Triangle> bowyerWatson(std::vector<p>& points) {
  
    Triangle superTriangle = createSuperTriangle(points);

    std::list<Triangle> triangulation;
    triangulation.push_back(superTriangle);

    for (const p& point : points) {
        std::list<Triangle> badTriangles;
        std::vector<Edge> polygon;

        // Find all the triangles that are no longer valid due to the insertion
        for (auto it = triangulation.begin(); it != triangulation.end();) {
            if (it->circumcircleContains(point)) {
                badTriangles.push_back(*it);
                it = triangulation.erase(it);
            }
            else {
                ++it;
            }
        }

        // Find the boundary of the polygonal hole
        for (const Triangle& t : badTriangles) {
            Edge e1(t.p1, t.p2);
            Edge e2(t.p2, t.p3);
            Edge e3(t.p3, t.p1);

            if (std::find(polygon.begin(), polygon.end(), e1) == polygon.end())
                polygon.push_back(e1);
            else
                polygon.erase(std::remove(polygon.begin(), polygon.end(), e1), polygon.end());

            if (std::find(polygon.begin(), polygon.end(), e2) == polygon.end())
                polygon.push_back(e2);
            else
                polygon.erase(std::remove(polygon.begin(), polygon.end(), e2), polygon.end());

            if (std::find(polygon.begin(), polygon.end(), e3) == polygon.end())
                polygon.push_back(e3);
            else
                polygon.erase(std::remove(polygon.begin(), polygon.end(), e3), polygon.end());
        }

        // Re-triangulate the polygonal hole
        for (const Edge& e : polygon) {
            triangulation.push_back(Triangle(point, e.p1, e.p2));
        }
    }

    // Remove triangles that share vertices with the super-triangle
    triangulation.remove_if([&superTriangle](const Triangle& t) {
        return (t.p1.x == superTriangle.p1.x && t.p1.y == superTriangle.p1.y) ||
            (t.p1.x == superTriangle.p2.x && t.p1.y == superTriangle.p2.y) ||
            (t.p1.x == superTriangle.p3.x && t.p1.y == superTriangle.p3.y) ||
            (t.p2.x == superTriangle.p1.x && t.p2.y == superTriangle.p1.y) ||
            (t.p2.x == superTriangle.p2.x && t.p2.y == superTriangle.p2.y) ||
            (t.p2.x == superTriangle.p3.x && t.p2.y == superTriangle.p3.y) ||
            (t.p3.x == superTriangle.p1.x && t.p3.y == superTriangle.p1.y) ||
            (t.p3.x == superTriangle.p2.x && t.p3.y == superTriangle.p2.y) ||
            (t.p3.x == superTriangle.p3.x && t.p3.y == superTriangle.p3.y);
        });

    return std::vector<Triangle>(triangulation.begin(), triangulation.end());
}

vector<p> trToLines(vector<Triangle> triangles) {
    vector<p> set;
    set.reserve(triangles.size() * 4);
    for (const auto& tr : triangles)
    {
        set.insert(set.end(), { tr.p1,tr.p2,tr.p3,tr.p1 });
    }
    return set;
}