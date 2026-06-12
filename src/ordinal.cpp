#include "ordinal.h"

#include <stdexcept>

OrdinalIndex::OrdinalIndex() : omega_count(0), finite_index(0) {}

OrdinalIndex::OrdinalIndex(int omega_count, int finite_index)
    : omega_count(omega_count), finite_index(finite_index) {
    if (omega_count < 0) {
        throw std::out_of_range("Omega count cannot be negative");
    }
    if (finite_index < 0) {
        throw std::out_of_range("Finite index cannot be negative");
    }
}

OrdinalIndex OrdinalIndex::finite(int index) { return OrdinalIndex(0, index); }

OrdinalIndex OrdinalIndex::omega_plus(int finite_index) {
    return OrdinalIndex(1, finite_index);
}

OrdinalIndex OrdinalIndex::omega_times(int omega_count, int finite_index) {
    if (omega_count <= 0) {
        throw std::out_of_range("Omega count must be positive");
    }
    return OrdinalIndex(omega_count, finite_index);
}

bool OrdinalIndex::is_finite() const { return omega_count == 0; }

bool OrdinalIndex::is_after_omega() const { return omega_count > 0; }

int OrdinalIndex::get_omega_count() const { return omega_count; }

int OrdinalIndex::get_finite_index() const { return finite_index; }

bool OrdinalIndex::operator==(const OrdinalIndex &other) const {
    return omega_count == other.omega_count && finite_index == other.finite_index;
}

bool OrdinalIndex::operator!=(const OrdinalIndex &other) const {
    return !(*this == other);
}

OrdinalLength::OrdinalLength() : omega_count(0), finite_count(0) {}

OrdinalLength::OrdinalLength(int omega_count, int finite_count)
    : omega_count(omega_count), finite_count(finite_count) {
    if (omega_count < 0) {
        throw std::out_of_range("Omega count cannot be negative");
    }
    if (finite_count < 0) {
        throw std::out_of_range("Finite count cannot be negative");
    }
}

OrdinalLength OrdinalLength::finite(int count) { return OrdinalLength(0, count); }

OrdinalLength OrdinalLength::omega() { return OrdinalLength(1, 0); }

OrdinalLength OrdinalLength::omega_times(int omega_count, int finite_count) {
    if (omega_count <= 0) {
        throw std::out_of_range("Omega count must be positive");
    }
    return OrdinalLength(omega_count, finite_count);
}

bool OrdinalLength::is_finite() const { return omega_count == 0; }

bool OrdinalLength::is_infinite() const { return omega_count > 0; }

int OrdinalLength::get_omega_count() const { return omega_count; }

int OrdinalLength::get_finite_count() const { return finite_count; }

bool OrdinalLength::contains(const OrdinalIndex &index) const {
    if (index.is_finite()) {
        return is_infinite() || index.get_finite_index() < finite_count;
    }
    if (is_finite()) {
        return false;
    }
    if (index.get_omega_count() < omega_count) {
        return true;
    }
    if (index.get_omega_count() == omega_count) {
        return index.get_finite_index() < finite_count;
    }
    return false;
}

OrdinalIndex OrdinalLength::subtract_prefix(const OrdinalIndex &index) const {
    if (contains(index)) {
        throw std::logic_error("Index is inside prefix");
    }
    if (is_finite()) {
        if (index.is_finite()) {
            return OrdinalIndex::finite(index.get_finite_index() - finite_count);
        }
        return index;
    }
    if (index.get_omega_count() == omega_count) {
        return OrdinalIndex::finite(index.get_finite_index() - finite_count);
    }
    return OrdinalIndex::omega_times(index.get_omega_count() - omega_count,
                                     index.get_finite_index());
}

OrdinalLength OrdinalLength::add(const OrdinalLength &left, const OrdinalLength &right) {
    if (right.is_finite()) {
        return OrdinalLength(left.omega_count, left.finite_count + right.finite_count);
    }
    return OrdinalLength(left.omega_count + right.omega_count, right.finite_count);
}
