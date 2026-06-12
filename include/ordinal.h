#pragma once

class OrdinalIndex {
  private:
    int omega_count;
    int finite_index;

    OrdinalIndex(int omega_count, int finite_index);

  public:
    static OrdinalIndex finite(int index);
    static OrdinalIndex omega_plus(int finite_index);
    static OrdinalIndex omega_times(int omega_count, int finite_index);

    bool is_finite() const;
    int get_omega_count() const;
    int get_finite_index() const;

    bool operator==(const OrdinalIndex &other) const;
};

class OrdinalLength {
  private:
    int omega_count;
    int finite_count;

    OrdinalLength(int omega_count, int finite_count);

  public:
    static OrdinalLength finite(int count);
    static OrdinalLength omega();
    static OrdinalLength omega_times(int omega_count, int finite_count);

    bool is_finite() const;
    bool is_infinite() const;
    int get_omega_count() const;
    int get_finite_count() const;

    bool contains(const OrdinalIndex &index) const;
    OrdinalIndex subtract_prefix(const OrdinalIndex &index) const;

    static OrdinalLength add(const OrdinalLength &left, const OrdinalLength &right);
};
