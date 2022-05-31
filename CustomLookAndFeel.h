/*
  ==============================================================================

    CustomLookAndFeel.h
    Created: 11 Apr 2022 12:52:41pm
    Author:  Easton Elting

  ==============================================================================
*/

#pragma once

class OtherLookAndFeel : public juce::LookAndFeel_V4
{
public:
    OtherLookAndFeel()
    {
        setColour (juce::Slider::thumbColourId, juce::Colours::red);
    }
    
    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                              const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider&) override
    {
        auto radius = (float) juce::jmin (width / 2, height / 2) - 4.0f;
        auto centreX = (float) x + (float) width  * 0.5f;
        auto centreY = (float) y + (float) height * 0.5f;
        auto rx = centreX - radius;
        auto ry = centreY - radius;
        auto rw = radius * 2.0f;
        auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
        
        // fill
        g.setColour (juce::Colours::white);
        g.fillEllipse (rx, ry, rw, rw);
 
        // outline
        g.setColour (juce::Colours::grey);
        g.drawEllipse (rx, ry, rw, rw, 2.0f);
        int innerLine = 25;
        g.drawEllipse (rx + innerLine / 2, ry + innerLine / 2, rw - innerLine, rw - innerLine, 2.0f);
        
        juce::Path p;
        auto pointerLength = radius; // * 0.33f;
        auto pointerThickness = 2.0f;
        p.addRectangle (-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
        p.applyTransform (juce::AffineTransform::rotation (angle).translated (centreX, centreY));
        
        // pointer
        g.setColour (juce::Colours::black);
        g.fillPath (p);
    }
};
