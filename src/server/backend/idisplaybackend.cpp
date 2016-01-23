#include "idisplaybackend.hpp"

IDisplayBackend::IDisplayBackend(unsigned int width, unsigned int height)
    : mWidth(width), mHeight(height) {}

unsigned int IDisplayBackend::getHeight() const {
    return mHeight;
}

unsigned int IDisplayBackend::getWidth() const {
    return mWidth;
}
