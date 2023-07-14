/**
 * The Register Base Class of the Translation Unit
 *
 * SPDX-License-Identifier: MIT
 *
 * Copyright (C) 2022, Reto Achermann (The University of British Columbia)
 */

#include <assert.h>

#include "logging.hpp"
#include "types.hpp"
#include "state_field_base.hpp"


StateFieldBase::StateFieldBase(std::string name, uint8_t nbits, uint64_t reset_value)
{
    if (nbits < 64) {
        this->_bitwidth = nbits;
        this->_mask     = (1ULL << nbits) - 1;
    } else {
        if (nbits > 64) {
            Logging::warn("StateFieldBase::StateFieldBase: bitwidth too large. Setting to 64\n");
        }
        this->_bitwidth = 64;
        this->_mask     = ~(0ULL);
    }
    this->_name        = name;
    this->_reset_value = reset_value & this->_mask;
    this->_value       = reset_value & this->_mask;

    this->_slices = std::map<std::string, std::pair<uint8_t, uint8_t>>();
}


void StateFieldBase::print_field(void)
{
    Logging::info("% 16s     % 2u    0x%016lx    (0x%llx)", this->_name.c_str(), this->_bitwidth,
                  this->_value, this->_reset_value);
}

bool StateFieldBase::add_slice(const std::string &name, uint8_t start, uint8_t end)
{
    if (this->_slices.contains(name)) {
        Logging::error("StateFieldBase::add_slice: slice %s already exists\n", name);
        return false;
    }

    if (start > end) {
        Logging::error("StateFieldBase::add_slice: start > end\n");
        return false;
    }

    // taking >= here, as the end bit is (N-1) for a bitwidth of N
    if (end >= this->_bitwidth) {
        Logging::error("StateFieldBase::add_slice: end > bitwidth\n");
        return false;
    }

    // do overlap check
    for (auto it = this->_slices.begin(); it != this->_slices.end(); it++) {
        // case 1: end is smaller than the start bit, no overlap
        if (it->second.first <= start && start <= it->second.second) {
            Logging::error("StateFieldBase::add_slice: start overlaps with existing slice "
                           "(start)\n");
            return false;
        }

        if (it->second.first <= end && end <= it->second.second) {
            Logging::error("StateFieldBase::add_slice: start overlaps with existing slice "
                           "(end)\n");
            return false;
        }
    }

    // add it to the slices
    this->_slices[name] = std::make_pair(start, end);

    // all set!
    return true;
}


uint64_t StateFieldBase::get_slice_value(const std::string &name)
{
    if (!this->_slices.contains(name)) {
        Logging::error("StateFieldBase::get_slice_value: slice %s does not exist\n", name);
        return false;
    }

    // get the slice
    auto slice = this->_slices[name];

    // need to handle the 64-bit case (second = 63, first = 0)
    uint8_t nbits = (slice.second - slice.first + 1);
    if (nbits == this->_bitwidth) {
        return this->_value & this->_mask;
    } else {
        assert(nbits < 64);
        uint64_t slice_mask = (1ULL << nbits) - 1;

        // extract the slice
        return (this->_value >> slice.first) & (slice_mask);
    }
}

bool StateFieldBase::set_slice_value(const std::string &name, uint64_t value)
{
    if (!this->_slices.contains(name)) {
        Logging::error("StateFieldBase::set_slice_value: slice %s does not exist\n", name);
        return false;
    }

    // get the slice
    auto slice = this->_slices[name];

    // need to handle the 64-bit case (second = 63, first = 0)
    uint8_t nbits = (slice.second - slice.first + 1);
    if (nbits == this->_bitwidth) {
        this->_value = value & this->_mask;
    } else {
        assert(nbits < 64);
        uint64_t slice_mask = (1ULL << nbits) - 1;

        // update the slice value
        this->_value = (this->_value & ~(slice_mask << slice.first))
                       | ((value & slice_mask) << slice.first);
    }

    return true;
}
