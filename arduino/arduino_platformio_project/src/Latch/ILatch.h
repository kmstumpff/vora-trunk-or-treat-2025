#pragma once

/**
 * @brief Abstract interface for latch control
 * 
 * Implement this interface to define how your specific latch hardware
 * should be opened and closed (servo, solenoid, motor, etc.)
 */
class ILatch {
private:
    bool _isOpen;

protected:
    void setOpen(bool open) {
        this->_isOpen = open;
    }

public:
    virtual ~ILatch() = default;
    
    /**
     * @brief Get current latch state
     * @return true if open, false if closed
     */
    bool isOpen() const { return this->_isOpen; };
    
    /**
     * @brief Open the latch
     * @return true if successful, false otherwise
     */
    virtual bool open() = 0;
    
    /**
     * @brief Close the latch
     * @return true if successful, false otherwise
     */
    virtual bool close() = 0;
    
    /**
     * @brief Initialize the latch hardware
     * @return true if successful, false otherwise
     */
    virtual bool begin() = 0;
};
