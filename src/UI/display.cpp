#include "display.h"

void Display::Update() {
    
    interface_->UpdateDisplay();
    display_->Update();

}