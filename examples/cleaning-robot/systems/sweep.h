#pragma once

#include <vector>
#include <corn/ecs/component.h>
#include <corn/ecs/system.h>

class SSweep : public corn::System {
public:
    SSweep(corn::Scene& scene, const std::vector<corn::CPolygon*>& objects);

    void update(float millis) override;

private:
    std::unordered_map<corn::CPolygon*, corn::CPolygon*> objects_;
};
