#ifndef LIVE_LINK_BASE_HPP
#define LIVE_LINK_BASE_HPP

#include "blend_shape_config.hpp"
#include "nlohmann/json.hpp"
#include <functional>
#include <string>
#include <vector>

using json = nlohmann::json;

namespace LiveLink {

class LiveLinkBase {
public:
    using SendCallback = std::function<void(const std::vector<char>& buffer)>;
    using Landmark = std::vector<std::vector<double>>;
    using BlendShape = std::vector<float>;

    explicit LiveLinkBase(const json& config) : json_(config) {}
    virtual ~LiveLinkBase() = default;

    virtual void Process(const SendCallback& callback, const Landmark& landmark) = 0;
    virtual void Encode() = 0;

    double Distance(const std::vector<double>& x, const std::vector<double>& y, bool threeD = false) const {
        if (threeD) {
            return std::pow(std::pow(x[0] - y[0], 2) + std::pow(x[1] - y[1], 2) + std::pow(x[2] - y[2], 2), 0.5);
        } else {
            return std::pow(std::pow(x[0] - y[0], 2) + std::pow(x[1] - y[1], 2), 0.5);
        }
    }

    double Clamp(double value, double min, double max) const {
        return std::max(std::min(value, max), min);
    }

    double Remap(double value, double min, double max) const {
        return (Clamp(value, min, max) - min) / (max - min);
    }

protected:
    const json& json_;
};

}  // namespace LiveLink

#endif