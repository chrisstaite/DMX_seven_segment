
#include "pin.h"
#include "port.h"
#include "const_pin.h"
#include "seven_segment_array.h"
#include "button.h"
#include "ws2812.h"
#include "dmx.h"
#include "ms_timer.h"
#include "eeprom.h"
#include "timed_event.h"
#include "save_event.h"
#include "rgb_display_event.h"
#include "repeat_event.h"
#include "latched_button.h"
#include "mode_event.h"
#include "controller.h"

#include <avr/interrupt.h>

namespace {

/// The number of milliseconds between the last change and
/// saving the values to the EEPROM
constexpr uint16_t SAVE_INTERVAL = 10000;

/// The number of milliseconds between the changing mode and
/// displaying the selected colour on the LED
constexpr uint16_t RGB_INTERVAL = 2000;

}  // anon namespace

int main()
{
    // Configure the buttons
    avr::Pin buttonUp{avr::ConstPin<avr::PortD, 6>::toPin()};
    avr::Pin buttonDown{avr::ConstPin<avr::PortD, 5>::toPin()};
    avr::Pin buttonMode{avr::ConstPin<avr::PortD, 7>::toPin()};
    avr::Button up{buttonUp};
    avr::Button down{buttonDown};
    avr::Button mode{buttonMode};

    // Configure the Display
    avr::Pin digitSelectors[3] = {
        {avr::ConstPin<avr::PortB, 2>::toPin()},
        {avr::ConstPin<avr::PortB, 1>::toPin()},
        {avr::ConstPin<avr::PortB, 0>::toPin()}
    };
	led::SevenSegmentArray display{digitSelectors, avr::PortC{}};

    // Configure millisecond timer
    avr::MsTimer timer{};

    // Configure DMX
    dmx::Dmx dmx{avr::ConstPin<avr::PortD, 2>{}};

    // Configure LED
    led::WS2812 ws2812{avr::ConstPin<avr::PortD, 3>{}};

    // Load the saved values
    avr::Eeprom eeprom{0};
    for (uint8_t i = 0; i < 3; ++i)
    {
        dmx.setChannel(i, eeprom[i]);
    }

    // Create the controller
    led::Controller controller{dmx, display};

    // Configure colour LED setter
    led::RgbDisplayEvent rgbDisplayEvent{ws2812, controller};
    avr::TimedEvent rgbDisplayTimer{rgbDisplayEvent};
    rgbDisplayTimer.reset(1);

    // Configure EEPROM saver
    led::SaveEvent saveEvent{eeprom, controller};
    avr::TimedEvent saveTimer{saveEvent};

    // Configure plus and minus buttons
    avr::RepeatEvent<led::Controller, &led::Controller::increase>
        increaseEvent{controller};
    avr::RepeatEvent<led::Controller, &led::Controller::decrease>
        decreaseEvent{controller};
    avr::TimedEvent increaseTimer{increaseEvent};
    avr::TimedEvent decreaseTimer{decreaseEvent};
    led::LatchedButton upLatch{up, increaseTimer};
    led::LatchedButton downLatch{down, decreaseTimer};
    
    // Configure mode button
    led::ModeEvent modeEvent{controller};
    avr::TimedEvent modeTimer{modeEvent};
    led::LatchedButton modeLatch{mode, modeTimer};

    // Globally enable interrupts
    sei();

    // The millis on the last loop
    uint16_t lastMillis = 0;
	while (1)
    {
        uint16_t loopMillis = timer.millis();
        if (lastMillis != loopMillis)
        {
            lastMillis = loopMillis;

            (void) saveTimer.tick();
            (void) rgbDisplayTimer.tick();

            if (increaseTimer.tick() || decreaseTimer.tick())
            {
                if (!rgbDisplayTimer.active())
                {
                    rgbDisplayTimer.reset(1);
                }
                saveTimer.reset(SAVE_INTERVAL);
            }

            if (modeTimer.tick())
            {
                uint8_t mode = modeEvent.mode();
                led::RGB ledColour {
                    mode == 0 ? static_cast<uint8_t>(255) : static_cast<uint8_t>(0),
                    mode == 1 ? static_cast<uint8_t>(255) : static_cast<uint8_t>(0),
                    mode == 2 ? static_cast<uint8_t>(255) : static_cast<uint8_t>(0)
                };
                ws2812.sendRgb(&ledColour, 1);
                rgbDisplayTimer.reset(RGB_INTERVAL);
            }

            upLatch.test();
            downLatch.test();
            modeLatch.test();
        }
	}

    // Never get here or bad things happen (chip stops)
	return 0;
}
