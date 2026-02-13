#pragma once

#include "../common.h"
#include "Event.h"

namespace EventCore
{

    /// \brief Detects up and down events from any boolean input.
    ///
    /// \author Alessandro Ribeiro
    ///
    class PressReleaseDetector
    {

    public:
        //
        // public state variables
        //
        bool down;    ///< true when occurs a transition from !pressed to pressed
        bool up;      ///< true when occurs a transition from pressed to !pressed
        bool pressed; ///< current pressed state

        //
        // events
        //
        /// \brief Called when a down event occurs
        ///
        /// Example of use with functions:
        ///
        /// \code
        ///
        /// void callbackFunction() {
        ///     ...
        /// }
        ///
        /// PressReleaseDetector right;
        /// right.OnDown.add( &callbackFunction );
        /// ...
        /// \endcode
        ///
        /// Example of use with method:
        ///
        /// \code
        ///
        /// class ExampleClass {
        /// public:
        ///     void callbackFunction(){
        ///         ...
        ///     }
        /// };
        ///
        /// ExampleClass obj;
        ///
        /// PressReleaseDetector right;
        /// right.OnDown.add( &obj, &ExampleClass::callbackFunction );
        /// ...
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        Event<void()> OnDown;

        /// \brief Called when an up event occurs
        ///
        /// Example of use with functions:
        ///
        /// \code
        ///
        /// void callbackFunction() {
        ///     ...
        /// }
        ///
        /// PressReleaseDetector right;
        /// right.OnUp.add( &callbackFunction );
        /// ...
        /// \endcode
        ///
        /// Example of use with method:
        ///
        /// \code
        ///
        /// class ExampleClass {
        /// public:
        ///     void callbackFunction(){
        ///         ...
        ///     }
        /// };
        ///
        /// ExampleClass obj;
        ///
        /// PressReleaseDetector right;
        /// right.OnUp.add( &obj, &ExampleClass::callbackFunction );
        /// ...
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        Event<void()> OnUp;

        PressReleaseDetector()
        {
            down = false;
            up = false;
            pressed = false;
        }

        /// \brief Set the current variable state
        ///
        /// Example:
        ///
        /// \code
        ///
        /// PressReleaseDetector right;
        ///
        /// void loop() {
        ///     right.setState( sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D) );
        ///
        ///     if ( right.down ) {
        ///         ...
        ///     }
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        void setState(bool v)
        {
            down = !pressed && v;
            up = pressed && !v;
            pressed = v;
            if (down)
                OnDown();
            if (up)
                OnUp();
        }
    };

}
