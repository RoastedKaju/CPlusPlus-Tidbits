#ifndef CRTP_POLYMORPHISM_H_
#define CRTP_POLYMORPHISM_H_

/// <summary>
/// CRTP (Curiously Recurring Template Pattern) gives you compile-time polymorphism — same “call derived behavior through base interface” feel as virtual functions, but:
///	no vtable, no virtual dispatch cost, fully inlineable, resolved at compile time
/// Tradeoff: you lose runtime polymorphism (no base pointers to mixed derived types).
/// </summary>

namespace crtp_poly
{
	// Virtual example
	struct Shape {
		virtual float area() const = 0;
		virtual ~Shape() {}
	};

	struct Circle : Shape {
		float radius;
		Circle(float radius) : radius{ radius } {}
		float area() const override {
			return 3.14f * radius * radius;
		}
	};

	// CRTP Example
	template<typename Derived>
	struct ShapeCRTP {
		float area() const { return derived().areaImpl(); }

	private:
		const Derived& derived() const {
			return static_cast<const Derived&>(*this);
		}
	};

	struct CircleCRTP : public ShapeCRTP<CircleCRTP>
	{
		float radius;
		CircleCRTP(float radius) : radius{ radius } {}

		float areaImpl() const {
			return 3.14f * radius * radius;
		}
	};

	// The better way to do polymorphism that is both compile and runtime is to use constexpr with virtual

	struct ShapeConstVirt
	{
		~ShapeConstVirt() = default;
		constexpr virtual float area() const = 0;
	};

	struct CircleConstVirt : public ShapeConstVirt
	{
		CircleConstVirt(float radius) : radius{ radius } {}

		float area() const override
		{
			return 3.14f * radius * radius;
		}

	private:
		float radius;
	};
}


#endif // !CRTP_POLYMORPHISM_H_
