#include <iostream>

/**
 * CRTP (Curiously Recurring Template Pattern)
 * It gives us compile time polymorphism - “call derived behavior through base interface” feel as virtual functions
 * However we have no overhead of V-tables, no virtual dispatch
 * it is fully compile time and in-line.
 * One trade off is you loose runtime polymorphism (no base pointers to mixed derived types)
 */

// First lets make a runtime virtual example
class Shape {
public:
    virtual ~Shape() = default;

    virtual float area() = 0;
};

// Derived circle class that inherits from Shape interface
class Circle final : public Shape {
public:
    explicit Circle(const float r) : radius_{r} {
    }

    float area() override {
        return 3.14f * radius_ * radius_;
    }

private:
    float radius_{0.0f};
};

// Now move onto CRTP
template<typename Derived>
class ShapeCompileTime {
public:
    float area() {
        return derived().area_impl();
    }

private:
    [[nodiscard]] const Derived &derived() const {
        return static_cast<const Derived &>(*this);
    }

    // Keep the constructor private
    ShapeCompileTime() = default;

    // This will make your life easier
    // You can now hide the implementation functions
    // It will also add a compile time check, when you are write the derived type
    friend Derived;
};

class CircleCompileTime : public ShapeCompileTime<CircleCompileTime> {
public:
    explicit CircleCompileTime(const float r) : radius_{r} {
    }

    [[nodiscard]] float area_impl() const {
        return 3.14f * radius_ * radius_;
    }

private:
    float radius_{0.0f};
};

// In modern C++ a better way to have both compile and run-time polymorphism by having just one version of code
// is to use constexpr
class ShapeConstExpr {
public:
    virtual ~ShapeConstExpr() = default;

    [[nodiscard]] constexpr virtual float area() const = 0;
};

class CircleConstExpr final : public ShapeConstExpr {
public:
    explicit CircleConstExpr(const float r) : radius_{r} {
    }

    ~CircleConstExpr() override = default;

    [[nodiscard]] float area() const override {
        return 3.14f * radius_ * radius_;
    }

private:
    float radius_{0.0f};
};


int main() {
    Circle circle{10.0f};
    std::cout << "Polymorphic circle type area is: " << circle.area() << std::endl;

    CircleCompileTime circle_compile{10.0f};
    std::cout << "CRTP area is: " << circle_compile.area() << std::endl;

    const CircleConstExpr circle_const_expr{10.0f};
    std::cout << "Constexpr area is: " << circle_const_expr.area() << std::endl;

    return EXIT_SUCCESS;
}
