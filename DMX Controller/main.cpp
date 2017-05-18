
#include "pin.h"
#include "port.h"
#include "const_pin.h"
#include "seven_segment.h"
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
#include "value_display_event.h"
#include "controller.h"

#include <avr/interrupt.h>
#include <avr/eeprom.h>

namespace {

/// The number of milliseconds between the last change and
/// saving the values to the EEPROM
constexpr uint16_t SAVE_INTERVAL = 10000;

/// The number of milliseconds between the changing mode and
/// displaying the selected colour on the LED
constexpr uint16_t RGB_INTERVAL = 2000;

/// The number of milliseconds between the chaning mode and
/// displaying the current channel value on the display
constexpr uint16_t CHANNEL_SHOW_INTERVAL = 800;

// Configure the buttons
avr::Pin buttonUp{avr::ConstPin<avr::PortD, 5>::toPin()};
avr::Pin buttonDown{avr::ConstPin<avr::PortD, 6>::toPin()};
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
dmx::Dmx dmxOutput{avr::ConstPin<avr::PortD, 2>{}};

// Configure LED
led::WS2812 ws2812{avr::ConstPin<avr::PortD, 3>{}};

// Set the default EEPROM
uint8_t channelStorage[] EEMEM = { 0, 0, 0, 0 };
// Load the saved values
avr::Eeprom eeprom{channelStorage};

// Create the controller
led::Controller controller{dmxOutput, display};

// Configure colour LED setter
led::RgbDisplayEvent rgbDisplayEvent{ws2812, controller};
avr::TimedEvent rgbDisplayTimer{rgbDisplayEvent};

// Configure timer to show the current value
led::ValueDisplayEvent valueDisplayEvent{controller};
avr::TimedEvent valueDisplayTimer{valueDisplayEvent};

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

}  // anon namespace

int main()
{
    for (uint8_t i = 0; i < led::ModeEvent::MAX_MODE; ++i)
    {
        dmxOutput.setChannel(i, eeprom[i]);
    }
    rgbDisplayTimer.reset(1);
    controller.setModifierChannel(0);

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
            (void) valueDisplayTimer.tick();

            if (increaseTimer.tick() || decreaseTimer.tick())
            {
                rgbDisplayTimer.reset(1);
                saveTimer.reset(SAVE_INTERVAL);
                valueDisplayTimer.cancel();
            }

            if (modeTimer.tick())
            {
                uint8_t mode = modeEvent.mode();
                led::RGB ledColour {
                    (mode == 0 || mode == 3) ? static_cast<uint8_t>(255) : static_cast<uint8_t>(0),
                    (mode == 1 || mode == 3) ? static_cast<uint8_t>(255) : static_cast<uint8_t>(0),
                    (mode == 2 || mode == 3) ? static_cast<uint8_t>(255) : static_cast<uint8_t>(0)
                };
                ws2812.sendRgb(&ledColour, 1);
                rgbDisplayTimer.reset(RGB_INTERVAL);

                display.setValue(0, led::SevenSegment::Letters::C);
                display.setValue(1, led::SevenSegment::Letters::H);
                display.setValue(2, mode + 1);
                valueDisplayTimer.reset(CHANNEL_SHOW_INTERVAL);
            }

            upLatch.test();
            downLatch.test();
            modeLatch.test();
        }
	}

    // Never get here or bad things happen (chip stops)
	return 0;
}
