#pragma once
#ifndef COMMAND_H
#define COMMAND_H

#include "buttonInterface.h"

class Command {
public:
    Command(){}
    virtual ~Command(){}

    virtual void Execute(buttonInterface*& interface, bool shift) = 0;
};

class ACommand : public Command {
public:
    ACommand(){}

    void Execute(buttonInterface*& interface, bool shift) {
        if (shift) interface->AltAButton();
        else interface->AButton();
    }
};

class BCommand : public Command {
public:
    BCommand(){}
    ~BCommand(){}

    void Execute(buttonInterface*& interface, bool shift) {
        if (shift) interface->AltBButton();
        else interface->BButton();
    }
};

class PlayCommand : public Command {
public:
    PlayCommand(){}
    ~PlayCommand(){}

    void Execute(buttonInterface*& interface, bool shift) {
        if (shift) interface->AltPlayButton();
        else interface->PlayButton();
    }
};

class UpCommand : public Command {
public:
    UpCommand(){}
    ~UpCommand(){}

    void Execute(buttonInterface*& interface, bool shift) {
        if (shift) interface->AltUpButton();
        else interface->UpButton();
    }
};

class DownCommand : public Command {
public:
    DownCommand(){}
    ~DownCommand(){}

    void Execute(buttonInterface*& interface, bool shift) {
        if (shift) interface->AltDownButton();
        else interface->DownButton();
    }
};

class LeftCommand : public Command {
public:
    LeftCommand(){}
    ~LeftCommand(){}

    void Execute(buttonInterface*& interface, bool shift) {
        if (shift) interface->AltLeftButton();
        else interface->LeftButton();
    }
};

class RightCommand : public Command {
public:
    RightCommand(){}
    ~RightCommand(){}

    void Execute(buttonInterface*& interface, bool shift) {
        if (shift) interface->AltRightButton();
        else interface->RightButton();
    }
};

class LeftShoulderCommand : public Command {
public:
    LeftShoulderCommand(){}
    ~LeftShoulderCommand(){}

    void Execute(buttonInterface*& interface, bool shift) {
        if (interface->GetPrev() != nullptr)
            interface = interface->GetPrev();
    }
};

class RightShoulderCommand : public Command {
public:
    RightShoulderCommand(){}
    ~RightShoulderCommand(){}

    void Execute(buttonInterface*& interface, bool shift) {
        if (interface->GetNext() != nullptr)
            interface = interface->GetNext();
    }
};


#endif