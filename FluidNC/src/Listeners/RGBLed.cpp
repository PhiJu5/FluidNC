#include "Platform.h"
#include "RGBLed.h"
#include "System.h"

namespace Listeners {
    RGBLed::RGBLed(const char *c) : SysListener("rgbled") {}

    void RGBLed::newStatus() {
        static State prevState = (State)-1;
        uint32_t stColor = statusFlip ? this->status : 0;

        if (!initDone )
            return;
        log_verbose("RGB:stColor=" << getColor(stColor));
        setColor(1, stColor);
        pixels_->show();
        if (prevState != sys.state())
             handleLed(SystemDirty::All);
        prevState = sys.state();
        statusFlip = !statusFlip;
    }

    void RGBLed::init() {
        if (this->pin_.defined() && number_) {
            auto thepin = this->pin_.getNative(Pin::Capabilities::Native | Pin::Capabilities::Output);
            log_debug("RGB Led gpio " << pin_ << ", number " << number_);
            pixels_ = new Adafruit_NeoPixel(4, thepin, NEO_GRB + NEO_KHZ800);
            pixels_->begin();
            pixels_->setBrightness(brightness_ & 0xFF);
            pixels_->show(); // Initialize all pixels to 'off'
            sys.register_change_handler(handleChange, this);
            initDone = true;
        }
    }

    void RGBLed::handleLed(SystemDirty changes) {
        uint32_t i = 2;
        uint32_t color = 0;
        //  2 1
        //  3 0
        //   |
        //  wire
        if (!initDone )
          return;
        log_debug("RGB" << (changes == SystemDirty::All ? ":" : ":changes=") << int(changes) << ":state=" << int(sys.state()));
        pixels_->clear();
        
        switch (sys.state()) {
            case State::Idle: // 0
                color = this->idle;
                break;
            case State::Alarm: // 1
                i = 3;
                color = this->alarm;
                break;
             case State::CheckMode: // 2
                color = this->checkMode;
                break;
             case State::Homing: // 3
                color = this->homing;
                break;
             case State::Cycle: // 4
                color = this->cycle;
                break;
             case State::Hold: // 5
                color = this->hold;
                break;
             case State::Held: // 6
                color = this->held;
                break;
             case State::Jog: // 7
                color = this->jog;
                break;
             case State::SafetyDoor: // 8
                i = 0;
                color = this->safetyDoor;
                break;
             case State::Sleep: // 9
                color = this->sleep;
                break;
             case State::ConfigAlarm: // 10
                i = 3;
                color = this->configAlarm;
                break;
             case State::Critical: // 11
                i = 3;
                color = this->critical;
                break;
            case State::Starting: // 12
                color = this->starting;
                break;
            default:
                color = this->none;
            break;
        }
        log_debug("RGB:" << i << ":" << getColor(color));
        setColor(i, color);
        pixels_->show();
    }
    // Configuration registration
    namespace {
        SysListenerFactory::InstanceBuilder<RGBLed> registration("rgbled");
    }
}

