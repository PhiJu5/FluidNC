#pragma once

#include "SysListener.h"
#include "../Pin.h"

#include <Adafruit_NeoPixel.h>

namespace Listeners {
    class RGBLed : public SysListener {
        Adafruit_NeoPixel* pixels_ = nullptr;
        Pin      pin_;
        uint32_t number_ = 0;
        uint32_t brightness_ = 32;
        bool initDone = false;
        bool statusFlip = false;

        std::string getColor(int32_t value) {
            if (value == -1) {
                return "none";
            } else {
                char buf[16];
                snprintf(buf, 16, "%02X%02X%02X", (value >> 16) & 0xFF, (value >> 8) & 0xff, value & 0xff);
                return buf;
            }
        }

        int32_t parseColor(const std::string& value, int32_t deft) {
            if (value == "none") {  // no change
                return -1;
            }

            if (value.size() != 6) {
                log_warn("Incorrect hex value: " << value);
                return deft;
            }

            int32_t v = 0;
            for (int i = 0; i < 3; ++i) {
                int x = 0;
                for (int j = 0; j < 2; ++j) {
                    char c = value[i * 2 + j];
                    if (c >= '0' && c <= '9') {
                        x = x * 16 + c - '0';
                    } else if (c >= 'a' && c <= 'f') {
                        x = x * 16 + c - 'a' + 10;
                    } else if (c >= 'A' && c <= 'F') {
                        x = x * 16 + c - 'A' + 10;
                    } else {
                        log_warn("Incorrect hex value: " << value);
                        return deft;
                    }
                }
                v = (v << 8) + x;
            }
            return v;
        }

        void handleChangeDetail(SystemDirty changes, const system_t& state) {
            handleLed(changes);
        }

        void handleLed(SystemDirty change);

        static void handleChange(SystemDirty changes, const system_t& state, void* userData) {
            static_cast<RGBLed*>(userData)->handleChangeDetail(changes, state);
        }

        // https://github.com/FastLED/FastLED/wiki/Pixel-reference
        uint32_t idle        = 0x0000FF; // Blue
        uint32_t alarm       = 0xFF0000; // Red
        uint32_t checkMode   = 0xFFC0CB; // Pink,  G-code check mode, locks out planner and motion only
        uint32_t homing      = 0xFFFF00; // Yellow
        uint32_t cycle       = 0x00FF00; // Green, Run
        uint32_t hold        = 0xFFA500; // Orange
        uint32_t held        = 0xFF8C00; // DarkOrange, feedhold complete
        uint32_t jog         = 0x800080; // Purple
        uint32_t safetyDoor  = 0xFF00FF; // Magenta
        uint32_t sleep       = 0xD2691E; // Chocolate
        uint32_t configAlarm = 0xC71585; // MediumVioletRed, You can't do anything but fix your config file
        uint32_t critical    = 0xFF0000; // Red, You can't do anything, reset with CTRL-x or the reset button
        uint32_t starting    = 0x40E0D0; // Turquoise
        uint32_t none        = 0xFFFFFF; // White
        uint32_t status      = 0x00FFFF; // Cyan

        void handleRGBString(Configuration::HandlerBase& handler, const char* name, uint32_t& value) {
            auto        old = value;
            std::string str = getColor(old);
            handler.item(name, str);
            value = parseColor(str, old);
        }

        void setColor(uint32_t i, int32_t color) {
            pixels_->setPixelColor(i, pixels_->Color((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF));
        }

    public:
        RGBLed(const char *c);
        void statusLed(void);
        virtual void group(Configuration::HandlerBase& handler) override {
            handler.item("pin", pin_);
            handler.item("number", number_);
            handler.item("brightness", brightness_);

            handleRGBString(handler, "idle", idle);
            handleRGBString(handler, "alarm", alarm);
            handleRGBString(handler, "checkMode", checkMode);
            handleRGBString(handler, "homing", homing);
            handleRGBString(handler, "cycle", cycle);
            handleRGBString(handler, "hold", hold);
            handleRGBString(handler, "held", held);
            handleRGBString(handler, "jog", jog);
            handleRGBString(handler, "safetyDoor", safetyDoor);
            handleRGBString(handler, "sleep", sleep);
            handleRGBString(handler, "configAlarm", configAlarm);
            handleRGBString(handler, "critical", critical);
            handleRGBString(handler, "starting", starting);
            handleRGBString(handler, "none", none);
            handleRGBString(handler, "status", status);
        }

        void init() override;
        void newStatus() override;
    };
}
