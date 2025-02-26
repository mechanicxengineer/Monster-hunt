
#pragma once

#define FMT_HEADER_ONLY 1
#include "../Externals/fmt/core.h"

#include "UObject/ReflectedTypeAccessors.h"


template<class T> inline
FName AdvEnumToFName(const T& enumValue) {
	return StaticEnum<T>()->GetNameByValue(static_cast<int64>(enumValue));
}

class AAAMECHANICS_API AdvFormat_FStringBackInserter {
	using This = AdvFormat_FStringBackInserter;
public:
	AdvFormat_FStringBackInserter() = default;
	explicit AdvFormat_FStringBackInserter(FString& s) noexcept : _s(&s) {}

	This& operator=(const char& _Val)		{ _s->AppendChar(_Val); return *this; }
	This& operator=(typename char && _Val)	{ _s->AppendChar(std::move(_Val)); return *this; }
	This& operator=(const wchar_t& _Val)	{ _s->AppendChar(_Val); return *this; }

	This& operator*() noexcept { return *this; }
	This& operator++() noexcept { return *this; }
	This  operator++(int) noexcept { return *this; }

	FString* _s;
};

template <> struct fmt::detail::is_output_iterator<AdvFormat_FStringBackInserter, char> : std::true_type {};

template<class... ARGS> inline
void AdvAppendFormat(FString& outStr, const char* format_str, const ARGS& ... args) {
	try {
		fmt::format_to(AdvFormat_FStringBackInserter(outStr), format_str, args...);
	} catch (...) {
		UE_LOG(LogTemp, Error, TEXT("Exception in Advance Format"));
	}
}

template<class... ARGS> inline
FString AdvFormat(const char* format_str, const ARGS& ... args) {
	FString tmp;
	AdvAppendFormat(tmp, format_str, args...);
	return tmp;
}

template<class... ARGS> inline
FText AdvFormatText(const char* format_str, const ARGS& ... args) {
	FString tmp;
	AdvAppendFormat(tmp, format_str, args...);
	return FText::FromString(tmp);
}

template<class... ARGS> inline
FName AdvFormatName(const char* format_str, const ARGS& ... args) {
	FString tmp;
	AdvAppendFormat(tmp, format_str, args...);
	return FName(tmp);
}

struct AdvFormatterBase {
	auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); }
};

template<>
struct fmt::formatter<wchar_t> : public AdvFormatterBase {
	auto format(const wchar_t& v, fmt::format_context& ctx) {
		return ctx.out();
	}
};

template<>
struct fmt::formatter<FString> : public AdvFormatterBase {
	auto format(const FString& v, fmt::format_context& ctx) {
//		const auto& arr = v.GetCharArray();
//		fmt::string_view view(arr.GetData(), arr.Num());
		auto it = *ctx.out();
		for (const auto& c :v.GetCharArray()) {
			it = static_cast<char>(c);
			it++;
		}
		return ctx.out();
	}
};

template<>
struct fmt::formatter<FName> : fmt::formatter<FString> {
	auto format(const FName& v, fmt::format_context& ctx) {
		return fmt::formatter<FString>::format(v.ToString(), ctx);
	}
};

template<>
struct fmt::formatter<FSmartName> : fmt::formatter<FString> {
	auto format(const FSmartName& v, fmt::format_context& ctx) {
		return fmt::formatter<FString>::format(v.DisplayName.ToString(), ctx);
	}
};

template<>
struct fmt::formatter<FText> : fmt::formatter<FString> {
	auto format(const FText& v, fmt::format_context& ctx) {
		return fmt::formatter<FString>::format(v.ToString(), ctx);
	}
};

template<>
struct fmt::formatter<FVector2D> : public AdvFormatterBase {
	auto format(const FVector2D& v, fmt::format_context& ctx) {
		return fmt::format_to(ctx.out(), "[{}, {}]", v.X, v.Y);
	}
};

template<>
struct fmt::formatter<FVector> : public AdvFormatterBase {
	auto format(const FVector& v, fmt::format_context& ctx) {
		return fmt::format_to(ctx.out(), "[{}, {}, {}]", v.X, v.Y, v.Z);
	}
};

template<>
struct fmt::formatter<FVector_NetQuantize> : public fmt::formatter<FVector> {
	auto format(const FVector_NetQuantize& v, fmt::format_context& ctx) {
		return fmt::formatter<FVector>::format(v, ctx);
	}
};

template<>
struct fmt::formatter<FVector4> : public AdvFormatterBase {
	auto format(const FVector4& v, fmt::format_context& ctx) {
		return fmt::format_to(ctx.out(), "[{}, {}, {}, {}]", v.X, v.Y, v.Z, v.W);
	}
};

template<>
struct fmt::formatter<FRotator> : public AdvFormatterBase {
	auto format(const FRotator& v, fmt::format_context& ctx) {
		return fmt::format_to(ctx.out(), "[{}, {}, {}]", v.Pitch, v.Yaw, v.Roll);
	}
};

template<>
struct fmt::formatter<AActor> : public AdvFormatterBase {
	auto format(const AActor& v, fmt::format_context& ctx) {
		return fmt::format_to(ctx.out(), "Actor:{}", &v == nullptr ? v.GetFName() : "");
	}
};