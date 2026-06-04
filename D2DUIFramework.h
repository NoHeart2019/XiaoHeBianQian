#pragma once

#include <d2d1.h>
#include <dwrite.h>
#include <atlcomcli.h>
#include <vector>
#include <algorithm>

/*
用 Direct2D 实现一组常见 CSS 功能，适用于简单 UI 控件

CD2DBorder：边框样式（四条边，线型，圆角）

CD2DBackground：背景（纯色，线性渐变，径向渐变）

CD2DStatus：状态管理（位标志，为每个状态存储背景、边框、颜色等，并提供回退逻辑）

CD2DTransform：矩阵变换

CD2DBoxModel：沙盒模型

CD2DSizePolicy: 尺寸策略

CD2DControlUI：多重继承上述类，作为控件基类

修复记录:
1. CD2DStatus::GetEffectiveXXX() 现在正确实现回退逻辑
2. CD2DBorder 添加默认构造，初始化所有成员
3. CD2DBackground 添加默认构造，初始化所有成员
4. CD2DBoxModel 添加初始化逻辑
5. CD2DTransform 补充完整的变换操作
6. CD2DRender 为指针添加初始化
7. 使用 static constexpr 替代宏定义
8. 添加完整的实现代码
*/

// CSS: border
class CD2DBorder
{
public:
	enum ELineStyle
	{
		None = 0,
		Solid,
		Dashed,
		Dotted,
		Double,
		// ... 可扩展
	};
	
	struct CEdge
	{
		float m_width;
		ELineStyle m_style;
		D2D1_COLOR_F m_color;
		float dashLength;   // dashed/dotted 时有效
		float gapLength;    // dashed 时有效

		CEdge() : m_width(0.0f), m_style(None), dashLength(3.0f), gapLength(3.0f)
		{
			m_color = D2D1::ColorF(D2D1::ColorF::Black);
		}

		bool IsVisible() const { return m_style != None && m_width > 0.0f; }
	};
	
public:
	CD2DBorder();
	virtual ~CD2DBorder() = default;

	//边框是否可见
	bool IsBorderVisible() const;

public:
	// 统一设置四边（宽、样式、颜色）
	void SetBorder(float width, ELineStyle style, const D2D1_COLOR_F& color);
	// 设置圆角（四角统一）
	void SetBorderRadius(float radius);
	void SetBorderRadius(float radiusX, float radiusY);

	// 单独设置某个角的圆角
	void SetBorderTopLeftRadius(float radiusX, float radiusY);
	void SetBorderTopRightRadius(float radiusX, float radiusY);
	void SetBorderBottomRightRadius(float radiusX, float radiusY);
	void SetBorderBottomLeftRadius(float radiusX, float radiusY);
	
	// 单独修改某条边
	void SetBorderTop(float width, ELineStyle style, const D2D1_COLOR_F& color);
	void SetBorderRight(float width, ELineStyle style, const D2D1_COLOR_F& color);
	void SetBorderBottom(float width, ELineStyle style, const D2D1_COLOR_F& color);
	void SetBorderLeft(float width, ELineStyle style, const D2D1_COLOR_F& color);

	// 获取方法
	const CEdge& GetBorderTop() const { return m_borderTop; }
	const CEdge& GetBorderRight() const { return m_borderRight; }
	const CEdge& GetBorderBottom() const { return m_borderBottom; }
	const CEdge& GetBorderLeft() const { return m_borderLeft; }

	const D2D1_POINT_2F& GetTopLeftRadius() const { return m_topLeftRadius; }
	const D2D1_POINT_2F& GetTopRightRadius() const { return m_topRightRadius; }
	const D2D1_POINT_2F& GetBottomRightRadius() const { return m_bottomRightRadius; }
	const D2D1_POINT_2F& GetBottomLeftRadius() const { return m_bottomLeftRadius; }

private:
	D2D1_POINT_2F m_topLeftRadius;
	D2D1_POINT_2F m_topRightRadius;
	D2D1_POINT_2F m_bottomRightRadius;
	D2D1_POINT_2F m_bottomLeftRadius;

	CEdge  m_borderTop;
	CEdge  m_borderRight;
	CEdge  m_borderBottom;
	CEdge  m_borderLeft;
};

// 实现 CD2DBorder
inline CD2DBorder::CD2DBorder()
{
	m_topLeftRadius = D2D1::Point2F(0.0f, 0.0f);
	m_topRightRadius = D2D1::Point2F(0.0f, 0.0f);
	m_bottomRightRadius = D2D1::Point2F(0.0f, 0.0f);
	m_bottomLeftRadius = D2D1::Point2F(0.0f, 0.0f);
}

inline bool CD2DBorder::IsBorderVisible() const
{
	return m_borderTop.IsVisible() || m_borderRight.IsVisible() || 
		   m_borderBottom.IsVisible() || m_borderLeft.IsVisible();
}

inline void CD2DBorder::SetBorder(float width, ELineStyle style, const D2D1_COLOR_F& color)
{
	m_borderTop.m_width = width;
	m_borderTop.m_style = style;
	m_borderTop.m_color = color;

	m_borderRight.m_width = width;
	m_borderRight.m_style = style;
	m_borderRight.m_color = color;

	m_borderBottom.m_width = width;
	m_borderBottom.m_style = style;
	m_borderBottom.m_color = color;

	m_borderLeft.m_width = width;
	m_borderLeft.m_style = style;
	m_borderLeft.m_color = color;
}

inline void CD2DBorder::SetBorderRadius(float radius)
{
	SetBorderRadius(radius, radius);
}

inline void CD2DBorder::SetBorderRadius(float radiusX, float radiusY)
{
	m_topLeftRadius = D2D1::Point2F(radiusX, radiusY);
	m_topRightRadius = D2D1::Point2F(radiusX, radiusY);
	m_bottomRightRadius = D2D1::Point2F(radiusX, radiusY);
	m_bottomLeftRadius = D2D1::Point2F(radiusX, radiusY);
}

inline void CD2DBorder::SetBorderTopLeftRadius(float radiusX, float radiusY)
{
	m_topLeftRadius = D2D1::Point2F(radiusX, radiusY);
}

inline void CD2DBorder::SetBorderTopRightRadius(float radiusX, float radiusY)
{
	m_topRightRadius = D2D1::Point2F(radiusX, radiusY);
}

inline void CD2DBorder::SetBorderBottomRightRadius(float radiusX, float radiusY)
{
	m_bottomRightRadius = D2D1::Point2F(radiusX, radiusY);
}

inline void CD2DBorder::SetBorderBottomLeftRadius(float radiusX, float radiusY)
{
	m_bottomLeftRadius = D2D1::Point2F(radiusX, radiusY);
}

inline void CD2DBorder::SetBorderTop(float width, ELineStyle style, const D2D1_COLOR_F& color)
{
	m_borderTop.m_width = width;
	m_borderTop.m_style = style;
	m_borderTop.m_color = color;
}

inline void CD2DBorder::SetBorderRight(float width, ELineStyle style, const D2D1_COLOR_F& color)
{
	m_borderRight.m_width = width;
	m_borderRight.m_style = style;
	m_borderRight.m_color = color;
}

inline void CD2DBorder::SetBorderBottom(float width, ELineStyle style, const D2D1_COLOR_F& color)
{
	m_borderBottom.m_width = width;
	m_borderBottom.m_style = style;
	m_borderBottom.m_color = color;
}

inline void CD2DBorder::SetBorderLeft(float width, ELineStyle style, const D2D1_COLOR_F& color)
{
	m_borderLeft.m_width = width;
	m_borderLeft.m_style = style;
	m_borderLeft.m_color = color;
}


class CD2DBackground
{
public:
	
	enum EType
	{
		None = 0,
		Solid,
		LinearGradient,
		RadialGradient   // 可扩展
	};

	struct SolidColor
	{
		D2D1_COLOR_F color;

		SolidColor() : color(D2D1::ColorF(D2D1::ColorF::White)) {}
	};

	struct GradientStop
	{
		float position;         // 0.0 ~ 1.0
		D2D1_COLOR_F color;

		GradientStop() : position(0.0f), color(D2D1::ColorF(D2D1::ColorF::Black)) {}
	};

	struct LinearGradient
	{
		D2D1_POINT_2F startPoint;
		D2D1_POINT_2F endPoint;
		D2D1_GRADIENT_STOP m_backgroundStops[10];
		int m_backgroundStopsCount;

		LinearGradient() : m_backgroundStopsCount(0)
		{
			startPoint = D2D1::Point2F(0.0f, 0.0f);
			endPoint = D2D1::Point2F(0.0f, 0.0f);
		}
	};

	struct RadialGradient
	{
		D2D1_POINT_2F center;
		D2D1_POINT_2F gradientOriginOffset;
		float radiusX;
		float radiusY;
		D2D1_GRADIENT_STOP m_backgroundStops[10];
		int m_backgroundStopsCount;

		RadialGradient() : radiusX(0.0f), radiusY(0.0f), m_backgroundStopsCount(0)
		{
			center = D2D1::Point2F(0.0f, 0.0f);
			gradientOriginOffset = D2D1::Point2F(0.0f, 0.0f);
		}
	};

	CD2DBackground();
	virtual ~CD2DBackground() = default;

	void SetSolidColor(const D2D1_COLOR_F& color);
	
	// 线性渐变：直接设置起点、终点和停止点
	void SetLinearGradient(const D2D1_POINT_2F& start, const D2D1_POINT_2F& end, 
						  const D2D1_GRADIENT_STOP stops[], int nStop);
	
	// 便捷版本：通过角度和矩形设置线性渐变
	void SetLinearGradientByAngle(float angle, const D2D1_RECT_F& rect,
								  const D2D1_GRADIENT_STOP stops[], int nStop);

	// 径向渐变：完整设置几何参数和停止点
	void SetRadialGradient(const D2D1_POINT_2F& center, const D2D1_POINT_2F& originOffset,
		float radiusX, float radiusY,
		const D2D1_GRADIENT_STOP stops[], int nStop);

	bool IsBackgroundVisible() const;

	EType GetBackgroundType() const { return m_backgroundType; }
	const SolidColor& GetSolidColor() const { return m_solid; }
	const LinearGradient& GetLinearGradient() const { return m_linear; }
	const RadialGradient& GetRadialGradient() const { return m_radial; }
	
private:
	EType m_backgroundType;
	union
	{
		SolidColor m_solid;
		LinearGradient m_linear;
		RadialGradient m_radial;
	};
};

// 实现 CD2DBackground
inline CD2DBackground::CD2DBackground() : m_backgroundType(None)
{
	m_solid = SolidColor();
}

inline void CD2DBackground::SetSolidColor(const D2D1_COLOR_F& color)
{
	m_backgroundType = Solid;
	m_solid.color = color;
}

inline void CD2DBackground::SetLinearGradient(const D2D1_POINT_2F& start, const D2D1_POINT_2F& end, 
											   const D2D1_GRADIENT_STOP stops[], int nStop)
{
	m_backgroundType = LinearGradient;
	m_linear.startPoint = start;
	m_linear.endPoint = end;
	m_linear.m_backgroundStopsCount = std::min(nStop, 10);
	for (int i = 0; i < m_linear.m_backgroundStopsCount; ++i)
	{
		m_linear.m_backgroundStops[i] = stops[i];
	}
}

inline void CD2DBackground::SetLinearGradientByAngle(float angle, const D2D1_RECT_F& rect,
													  const D2D1_GRADIENT_STOP stops[], int nStop)
{
	// 根据角度计算起点和终点
	const float PI = 3.14159265358979f;
	float radian = angle * PI / 180.0f;
	
	float width = rect.right - rect.left;
	float height = rect.bottom - rect.top;
	float diagonal = std::sqrt(width * width + height * height) / 2.0f;
	
	D2D1_POINT_2F center = D2D1::Point2F(
		rect.left + width / 2.0f,
		rect.top + height / 2.0f
	);
	
	D2D1_POINT_2F start = D2D1::Point2F(
		center.x - diagonal * std::cos(radian),
		center.y - diagonal * std::sin(radian)
	);
	
	D2D1_POINT_2F end = D2D1::Point2F(
		center.x + diagonal * std::cos(radian),
		center.y + diagonal * std::sin(radian)
	);
	
	SetLinearGradient(start, end, stops, nStop);
}

inline void CD2DBackground::SetRadialGradient(const D2D1_POINT_2F& center, const D2D1_POINT_2F& originOffset,
											   float radiusX, float radiusY,
											   const D2D1_GRADIENT_STOP stops[], int nStop)
{
	m_backgroundType = RadialGradient;
	m_radial.center = center;
	m_radial.gradientOriginOffset = originOffset;
	m_radial.radiusX = radiusX;
	m_radial.radiusY = radiusY;
	m_radial.m_backgroundStopsCount = std::min(nStop, 10);
	for (int i = 0; i < m_radial.m_backgroundStopsCount; ++i)
	{
		m_radial.m_backgroundStops[i] = stops[i];
	}
}

inline bool CD2DBackground::IsBackgroundVisible() const
{
	return m_backgroundType != None;
}


//位掩码枚举 支持多状态同时存在
//设置normal的时，给数组全部初始化。
class CD2DStatus
{
public:
	static constexpr int StatusCount = 9;

	enum EFlag {
		None        = 0,                    // 0
		Normal      = 1 << 0,               // 0x0001
		Hover       = 1 << 1,               // 0x0002  鼠标悬浮
		Pressed     = 1 << 2,               // 0x0004  鼠标按下
		Disabled    = 1 << 3,               // 0x0008  禁用
		Focused     = 1 << 4,               // 0x0010  键盘焦点
		Dragging    = 1 << 5,               // 0x0020  拖拽中
		Checked     = 1 << 6,               // 0x0040  选中（Toggle）
		Active      = 1 << 7,               // 0x0080  激活（按下后未松开）
		HotTracked  = 1 << 8,               // 0x0100  热追踪（Hover + Pressed 组合）
	};

	CD2DStatus();
	virtual ~CD2DStatus() = default;

	// 检查某状态是否激活（可检查单个或组合）
	bool Is(int flag) const;
	// 设置/清除一个或多个状态
	void Set(int flag, bool on = true);

	void SetNormal(bool on = true);
	bool IsNormal() const;

	void SetHover(bool on = true);
	bool IsHover() const;

	void SetPressed(bool on = true);
	bool IsPressed() const;

	void SetDisabled(bool on = true);
	bool IsDisabled() const;

	void SetFocused(bool on = true);
	bool IsFocused() const;

	void SetDragging(bool on = true);
	bool IsDragging() const;

	void SetChecked(bool on = true);
	bool IsChecked() const;

	void SetActive(bool on = true);
	bool IsActive() const;

	void SetHotTracked(bool on = true);
	bool IsHotTracked() const;

	// 样式设置（只支持单个状态，不支持组合键）
	void SetBackground(unsigned int stateFlag, const CD2DBackground& bg);
	void SetBorder(unsigned int stateFlag, const CD2DBorder& border);
	void SetColor(unsigned int stateFlag, const D2D1_COLOR_F& color);

	// 获取有效样式（自动回退逻辑）
	const CD2DBackground* GetEffectiveBackground() const;
	const CD2DBorder*     GetEffectiveBorder() const;
	const D2D1_COLOR_F*   GetEffectiveColor() const;

private:
	// 辅助函数：查找有效状态的索引
	int GetStatusIndex(unsigned int stateFlag) const;

	unsigned int m_statusFlags;

	CD2DBackground m_backgrounds[StatusCount];
	CD2DBorder m_borders[StatusCount];
	D2D1_COLOR_F m_colors[StatusCount];
};

// 实现 CD2DStatus
inline CD2DStatus::CD2DStatus() : m_statusFlags(Normal)
{
	// 初始化所有颜色
	for (int i = 0; i < StatusCount; ++i)
	{
		m_colors[i] = D2D1::ColorF(D2D1::ColorF::Black);
	}
}

inline bool CD2DStatus::Is(int flag) const
{
	return (m_statusFlags & flag) != 0;
}

inline void CD2DStatus::Set(int flag, bool on)
{
	if (on)
		m_statusFlags |= flag;
	else
		m_statusFlags &= ~flag;
}

inline void CD2DStatus::SetNormal(bool on)
{
	Set(Normal, on);
}

inline bool CD2DStatus::IsNormal() const
{
	return Is(Normal);
}

inline void CD2DStatus::SetHover(bool on)
{
	Set(Hover, on);
}

inline bool CD2DStatus::IsHover() const
{
	return Is(Hover);
}

inline void CD2DStatus::SetPressed(bool on)
{
	Set(Pressed, on);
}

inline bool CD2DStatus::IsPressed() const
{
	return Is(Pressed);
}

inline void CD2DStatus::SetDisabled(bool on)
{
	Set(Disabled, on);
}

inline bool CD2DStatus::IsDisabled() const
{
	return Is(Disabled);
}

inline void CD2DStatus::SetFocused(bool on)
{
	Set(Focused, on);
}

inline bool CD2DStatus::IsFocused() const
{
	return Is(Focused);
}

inline void CD2DStatus::SetDragging(bool on)
{
	Set(Dragging, on);
}

inline bool CD2DStatus::IsDragging() const
{
	return Is(Dragging);
}

inline void CD2DStatus::SetChecked(bool on)
{
	Set(Checked, on);
}

inline bool CD2DStatus::IsChecked() const
{
	return Is(Checked);
}

inline void CD2DStatus::SetActive(bool on)
{
	Set(Active, on);
}

inline bool CD2DStatus::IsActive() const
{
	return Is(Active);
}

inline void CD2DStatus::SetHotTracked(bool on)
{
	Set(HotTracked, on);
}

inline bool CD2DStatus::IsHotTracked() const
{
	return Is(HotTracked);
}

inline int CD2DStatus::GetStatusIndex(unsigned int stateFlag) const
{
	// 将状态标志转换为数组索引
	switch (stateFlag)
	{
	case Normal:     return 0;
	case Hover:      return 1;
	case Pressed:    return 2;
	case Disabled:   return 3;
	case Focused:    return 4;
	case Dragging:   return 5;
	case Checked:    return 6;
	case Active:     return 7;
	case HotTracked: return 8;
	default:         return 0;  // 回退到 Normal
	}
}

inline void CD2DStatus::SetBackground(unsigned int stateFlag, const CD2DBackground& bg)
{
	int index = GetStatusIndex(stateFlag);
	m_backgrounds[index] = bg;
}

inline void CD2DStatus::SetBorder(unsigned int stateFlag, const CD2DBorder& border)
{
	int index = GetStatusIndex(stateFlag);
	m_borders[index] = border;
}

inline void CD2DStatus::SetColor(unsigned int stateFlag, const D2D1_COLOR_F& color)
{
	int index = GetStatusIndex(stateFlag);
	m_colors[index] = color;
}

inline const CD2DBackground* CD2DStatus::GetEffectiveBackground() const
{
	// 优先级回退逻辑：按状态优先级查找
	// Pressed > Focused > HotTracked > Hover > Checked > Active > Disabled > Normal
	const int priorityOrder[] = {
		Pressed, Focused, HotTracked, Hover, Checked, Active, Disabled, Normal
	};

	for (int state : priorityOrder)
	{
		if (m_statusFlags & state)
		{
			int index = GetStatusIndex(state);
			if (m_backgrounds[index].IsBackgroundVisible())
			{
				return &m_backgrounds[index];
			}
		}
	}

	// 最后回退到 Normal
	return &m_backgrounds[GetStatusIndex(Normal)];
}

inline const CD2DBorder* CD2DStatus::GetEffectiveBorder() const
{
	// 同样的回退逻辑
	const int priorityOrder[] = {
		Pressed, Focused, HotTracked, Hover, Checked, Active, Disabled, Normal
	};

	for (int state : priorityOrder)
	{
		if (m_statusFlags & state)
		{
			int index = GetStatusIndex(state);
			if (m_borders[index].IsBorderVisible())
			{
				return &m_borders[index];
			}
		}
	}

	return &m_borders[GetStatusIndex(Normal)];
}

inline const D2D1_COLOR_F* CD2DStatus::GetEffectiveColor() const
{
	// 同样的回退逻辑
	const int priorityOrder[] = {
		Pressed, Focused, HotTracked, Hover, Checked, Active, Disabled, Normal
	};

	for (int state : priorityOrder)
	{
		if (m_statusFlags & state)
		{
			int index = GetStatusIndex(state);
			return &m_colors[index];
		}
	}

	return &m_colors[GetStatusIndex(Normal)];
}


class CD2DTransform
{
public:
	// 变换类型
	enum EType
	{
		None = 0,
		Translate,   // 平移
		Rotate,      // 旋转（角度，单位：度）
		Scale,       // 缩放
		Skew         // 倾斜（X/Y 方向角度，单位：度）
	};

	// 单个变换操作
	struct TransformOp
	{
		EType type;
		float param1;   // translateX, rotateAngle, scaleX, skewX
		float param2;   // translateY, (rotate无), scaleY, skewY

		TransformOp() : type(None), param1(0.0f), param2(0.0f) {}
	};

	CD2DTransform();
	virtual ~CD2DTransform() = default;

	void SetTranslate(float x, float y);
	void SetRotate(float angle);
	void SetScale(float scaleX, float scaleY);
	void SetSkew(float skewX, float skewY);

	const TransformOp& GetTransform() const { return m_op; }
	D2D1_MATRIX_3X2_F ToMatrix() const;

private:
	TransformOp m_op;
};

// 实现 CD2DTransform
inline CD2DTransform::CD2DTransform()
{
	m_op = TransformOp();
}

inline void CD2DTransform::SetTranslate(float x, float y)
{
	m_op.type = Translate;
	m_op.param1 = x;
	m_op.param2 = y;
}

inline void CD2DTransform::SetRotate(float angle)
{
	m_op.type = Rotate;
	m_op.param1 = angle;
	m_op.param2 = 0.0f;
}

inline void CD2DTransform::SetScale(float scaleX, float scaleY)
{
	m_op.type = Scale;
	m_op.param1 = scaleX;
	m_op.param2 = scaleY;
}

inline void CD2DTransform::SetSkew(float skewX, float skewY)
{
	m_op.type = Skew;
	m_op.param1 = skewX;
	m_op.param2 = skewY;
}

inline D2D1_MATRIX_3X2_F CD2DTransform::ToMatrix() const
{
	const float PI = 3.14159265358979f;

	switch (m_op.type)
	{
	case Translate:
		return D2D1::Matrix3x2F::Translation(m_op.param1, m_op.param2);

	case Rotate:
	{
		float radian = m_op.param1 * PI / 180.0f;
		return D2D1::Matrix3x2F::Rotation(m_op.param1, D2D1::Point2F(0.0f, 0.0f));
	}

	case Scale:
		return D2D1::Matrix3x2F::Scale(m_op.param1, m_op.param2, D2D1::Point2F(0.0f, 0.0f));

	case Skew:
	{
		float skewX = m_op.param1 * PI / 180.0f;
		float skewY = m_op.param2 * PI / 180.0f;
		return D2D1::Matrix3x2F(
			1.0f, std::tan(skewY),
			std::tan(skewX), 1.0f,
			0.0f, 0.0f
		);
	}

	default:
	case None:
		return D2D1::Matrix3x2F::Identity();
	}
}


//使用CSS盒模型来实现
class CD2DBoxModel
{
public:
	CD2DBoxModel();
	virtual ~CD2DBoxModel() = default;

	// 盒模型相关方法
	// 外边距(Margin)相关方法
	void SetMargin(float left, float top, float right, float bottom);
	void SetMarginLeft(float margin);
	void SetMarginTop(float margin);
	void SetMarginRight(float margin);
	void SetMarginBottom(float margin);
	float GetMarginLeft() const;
	float GetMarginTop() const;
	float GetMarginRight() const;
	float GetMarginBottom() const;

	// 内边距(Padding)相关方法
	void SetPadding(float left, float top, float right, float bottom);
	void SetPadding(float padding);
	void SetPaddingXY(float x, float y);
	void SetPaddingLeft(float padding);
	void SetPaddingTop(float padding);
	void SetPaddingRight(float padding);
	void SetPaddingBottom(float padding);
	float GetPaddingLeft() const;
	float GetPaddingTop() const;
	float GetPaddingRight() const;
	float GetPaddingBottom() const;

	// 边框(Border)相关方法
	void SetBorderSize(float left, float top, float right, float bottom);
	void SetBorderLeft(float border);
	void SetBorderTop(float border);
	void SetBorderRight(float border);
	void SetBorderBottom(float border);
	float GetBorderLeft() const;
	float GetBorderTop() const;
	float GetBorderRight() const;
	float GetBorderBottom() const;

	// 获取不同区域矩形的方法
	D2D1_RECT_F GetContentRectangle() const;    // 获取内容区域矩形
	D2D1_RECT_F GetPaddingRectangle() const;    // 获取内边距区域矩形（包含内容区域）
	D2D1_RECT_F GetBorderRectangle() const;     // 获取边框区域矩形（包含padding区域）
	D2D1_RECT_F GetMarginRectangle() const;     // 获取外边距区域矩形（包含所有内部区域）

	virtual D2D1_RECT_F GetRectangle() const;
	virtual D2D1_SIZE_F GetBoxModelSize() const;
	
	void SetBoxModel(float marginLeft, float marginTop, float marginRight, float marginBottom, 
		float borderLeft,  float borderTop,  float borderRight, float borderBottom, 
		float paddingLeft, float paddingTop, float paddingRight, float paddingBottom);

protected:
	// 设置基础矩形（由派生类实现）
	virtual void SetRectangleInternal(const D2D1_RECT_F& rect);

	// CSS盒模型相关成员变量
	// 外边距(Margin)
	float m_marginLeft;                 // 左边距
	float m_marginTop;                  // 上边距
	float m_marginRight;                // 右边距
	float m_marginBottom;               // 下边距

	// 内边距(Padding)
	float m_paddingLeft;                // 左内边距
	float m_paddingTop;                 // 上内边距
	float m_paddingRight;               // 右内边距
	float m_paddingBottom;              // 下内边距

	// 边框(Border)
	float m_borderLeft;                 // 左边框宽度
	float m_borderTop;                  // 上边框宽度
	float m_borderRight;                // 右边框宽度
	float m_borderBottom;               // 下边框宽度

	// 基础矩形（子类应该定义）
	D2D1_RECT_F m_baseRect;
};

// 实现 CD2DBoxModel
inline CD2DBoxModel::CD2DBoxModel()
	: m_marginLeft(0.0f), m_marginTop(0.0f), m_marginRight(0.0f), m_marginBottom(0.0f),
	  m_paddingLeft(0.0f), m_paddingTop(0.0f), m_paddingRight(0.0f), m_paddingBottom(0.0f),
	  m_borderLeft(0.0f), m_borderTop(0.0f), m_borderRight(0.0f), m_borderBottom(0.0f)
{
	m_baseRect = D2D1::RectF(0.0f, 0.0f, 0.0f, 0.0f);
}

inline void CD2DBoxModel::SetMargin(float left, float top, float right, float bottom)
{
	m_marginLeft = left;
	m_marginTop = top;
	m_marginRight = right;
	m_marginBottom = bottom;
}

inline void CD2DBoxModel::SetMarginLeft(float margin)
{
	m_marginLeft = margin;
}

inline void CD2DBoxModel::SetMarginTop(float margin)
{
	m_marginTop = margin;
}

inline void CD2DBoxModel::SetMarginRight(float margin)
{
	m_marginRight = margin;
}

inline void CD2DBoxModel::SetMarginBottom(float margin)
{
	m_marginBottom = margin;
}

inline float CD2DBoxModel::GetMarginLeft() const { return m_marginLeft; }
inline float CD2DBoxModel::GetMarginTop() const { return m_marginTop; }
inline float CD2DBoxModel::GetMarginRight() const { return m_marginRight; }
inline float CD2DBoxModel::GetMarginBottom() const { return m_marginBottom; }

inline void CD2DBoxModel::SetPadding(float left, float top, float right, float bottom)
{
	m_paddingLeft = left;
	m_paddingTop = top;
	m_paddingRight = right;
	m_paddingBottom = bottom;
}

inline void CD2DBoxModel::SetPadding(float padding)
{
	m_paddingLeft = m_paddingTop = m_paddingRight = m_paddingBottom = padding;
}

inline void CD2DBoxModel::SetPaddingXY(float x, float y)
{
	m_paddingLeft = m_paddingRight = x;
	m_paddingTop = m_paddingBottom = y;
}

inline void CD2DBoxModel::SetPaddingLeft(float padding) { m_paddingLeft = padding; }
inline void CD2DBoxModel::SetPaddingTop(float padding) { m_paddingTop = padding; }
inline void CD2DBoxModel::SetPaddingRight(float padding) { m_paddingRight = padding; }
inline void CD2DBoxModel::SetPaddingBottom(float padding) { m_paddingBottom = padding; }

inline float CD2DBoxModel::GetPaddingLeft() const { return m_paddingLeft; }
inline float CD2DBoxModel::GetPaddingTop() const { return m_paddingTop; }
inline float CD2DBoxModel::GetPaddingRight() const { return m_paddingRight; }
inline float CD2DBoxModel::GetPaddingBottom() const { return m_paddingBottom; }

inline void CD2DBoxModel::SetBorderSize(float left, float top, float right, float bottom)
{
	m_borderLeft = left;
	m_borderTop = top;
	m_borderRight = right;
	m_borderBottom = bottom;
}

inline void CD2DBoxModel::SetBorderLeft(float border) { m_borderLeft = border; }
inline void CD2DBoxModel::SetBorderTop(float border) { m_borderTop = border; }
inline void CD2DBoxModel::SetBorderRight(float border) { m_borderRight = border; }
inline void CD2DBoxModel::SetBorderBottom(float border) { m_borderBottom = border; }

inline float CD2DBoxModel::GetBorderLeft() const { return m_borderLeft; }
inline float CD2DBoxModel::GetBorderTop() const { return m_borderTop; }
inline float CD2DBoxModel::GetBorderRight() const { return m_borderRight; }
inline float CD2DBoxModel::GetBorderBottom() const { return m_borderBottom; }

inline D2D1_RECT_F CD2DBoxModel::GetContentRectangle() const
{
	D2D1_RECT_F marginRect = GetMarginRectangle();
	return D2D1::RectF(
		marginRect.left + m_marginLeft + m_borderLeft + m_paddingLeft,
		marginRect.top + m_marginTop + m_borderTop + m_paddingTop,
		marginRect.right - m_marginRight - m_borderRight - m_paddingRight,
		marginRect.bottom - m_marginBottom - m_borderBottom - m_paddingBottom
	);
}

inline D2D1_RECT_F CD2DBoxModel::GetPaddingRectangle() const
{
	D2D1_RECT_F marginRect = GetMarginRectangle();
	return D2D1::RectF(
		marginRect.left + m_marginLeft + m_borderLeft,
		marginRect.top + m_marginTop + m_borderTop,
		marginRect.right - m_marginRight - m_borderRight,
		marginRect.bottom - m_marginBottom - m_borderBottom
	);
}

inline D2D1_RECT_F CD2DBoxModel::GetBorderRectangle() const
{
	D2D1_RECT_F marginRect = GetMarginRectangle();
	return D2D1::RectF(
		marginRect.left + m_marginLeft,
		marginRect.top + m_marginTop,
		marginRect.right - m_marginRight,
		marginRect.bottom - m_marginBottom
	);
}

inline D2D1_RECT_F CD2DBoxModel::GetMarginRectangle() const
{
	return m_baseRect;
}

inline D2D1_RECT_F CD2DBoxModel::GetRectangle() const
{
	return GetMarginRectangle();
}

inline D2D1_SIZE_F CD2DBoxModel::GetBoxModelSize() const
{
	D2D1_RECT_F rect = GetRectangle();
	return D2D1::SizeF(rect.right - rect.left, rect.bottom - rect.top);
}

inline void CD2DBoxModel::SetBoxModel(float marginLeft, float marginTop, float marginRight, float marginBottom,
									   float borderLeft, float borderTop, float borderRight, float borderBottom,
									   float paddingLeft, float paddingTop, float paddingRight, float paddingBottom)
{
	SetMargin(marginLeft, marginTop, marginRight, marginBottom);
	SetBorderSize(borderLeft, borderTop, borderRight, borderBottom);
	SetPadding(paddingLeft, paddingTop, paddingRight, paddingBottom);
}

inline void CD2DBoxModel::SetRectangleInternal(const D2D1_RECT_F& rect)
{
	m_baseRect = rect;
}


class CD2DSizePolicy
{
public:
	enum Policy
	{
		Fixed = 0x0,        // 固定大小
		Minimum = 0x1,      // 最小大小
		Maximum = 0x2,      // 最大大小
		Preferred = 0x3,    // 首选大小 (默认)
		MinimumExpanding = 0x4, // 最小且可扩展 (已过时，但在源码中仍存在)
		Expanding = 0x5,    // 可扩展
		Ignored = 0x6       // 忽略 sizeHint
	};

	explicit CD2DSizePolicy(Policy size = Preferred) : m_policy(size) {}

	CD2DSizePolicy() : m_policy(Preferred) {}

	virtual ~CD2DSizePolicy() = default;

	// 提供接口来修改
	void SetPolicy(Policy p) { m_policy = p; }

	// 提供接口来获取
	Policy GetPolicy() const { return m_policy; }

	// 重载 == 运算符，支持直接比较对象
	bool operator==(Policy p) const { return m_policy == p; }
	bool operator!=(Policy p) const { return m_policy != p; }

private:
	Policy m_policy;
};


class CD2DRender
{
public:
	CD2DRender();
	virtual ~CD2DRender() = default;

	CComPtr<ID2D1Factory> GetFactory() const { return m_pFactory; }
	CComPtr<IDWriteFactory> GetDWriteFactory() const { return m_pDWriteFactory; }
	CComPtr<ID2D1HwndRenderTarget> GetRenderTarget() const { return m_pRenderTarget; }
	CComPtr<ID2D1DrawingStateBlock> GetDrawingStateBlock() const { return m_pDrawingStateBlock; }
	HWND GetHwnd() const { return m_hwnd; }
	float GetDpiScaleX() const { return m_dpiScaleX; }
	float GetDpiScaleY() const { return m_dpiScaleY; }

private:
	CComPtr<ID2D1Factory> m_pFactory;
	CComPtr<IDWriteFactory> m_pDWriteFactory;
	CComPtr<ID2D1HwndRenderTarget> m_pRenderTarget;
	CComPtr<ID2D1DrawingStateBlock> m_pDrawingStateBlock;
	HWND m_hwnd;
	float m_dpiScaleX;
	float m_dpiScaleY;
};

// 实现 CD2DRender
inline CD2DRender::CD2DRender() 
	: m_hwnd(nullptr), m_dpiScaleX(1.0f), m_dpiScaleY(1.0f)
{
	// m_pFactory, m_pDWriteFactory 等 CComPtr 自动初始化为 nullptr
}


class CD2DControlUI : public CD2DBoxModel, public CD2DStatus, public CD2DSizePolicy
{
public:
	CD2DControlUI();
	virtual ~CD2DControlUI() = default;

	void SetPosition(float x, float y);
	virtual void SetSize(float width, float height);
	void SetContentSize(float width, float height);
	void SetRectangle(const D2D1_RECT_F& rect);
	void SetCenter(float x, float y);

	virtual D2D1_RECT_F GetRectangle() const;
	virtual D2D1_ROUNDED_RECT GetRoundedRectangle() const;

	D2D1_POINT_2F GetPosition() const;
	D2D1_SIZE_F GetSize() const;
	D2D1_POINT_2F GetCenter() const;
	float GetWidth() const;
	float GetHeight() const;

	void Move(float offsetX, float offsetY);
	void MoveTo(float x, float y);
	void Inflate(float dx, float dy);

	void Resize(float newWidth, float newHeight);

	// 检查点是否在矩形内
	bool IsHitTest(const D2D1_POINT_2F& point, const D2D1_RECT_F& rect) const;
	virtual bool IsHitTest(const D2D1_POINT_2F& point) const;
	bool IsHitTest(float x, float y) const;
	
	// 设置用户数据
	void SetUserData(void* userData);
	void* GetUserData() const;

	// 获取边框样式
	CD2DBorder& GetBorder() { return m_border; }
	const CD2DBorder& GetBorder() const { return m_border; }

	// 获取背景样式
	CD2DBackground& GetBackground() { return m_background; }
	const CD2DBackground& GetBackground() const { return m_background; }

	// 获取变换
	CD2DTransform& GetTransform() { return m_transform; }
	const CD2DTransform& GetTransform() const { return m_transform; }

public:
	virtual bool OnMouseDownL(const D2D1_POINT_2F& point);
	virtual bool OnMouseMove(const D2D1_POINT_2F& point);
	virtual bool OnMouseUpL(const D2D1_POINT_2F& point);
	virtual bool OnMouseDownL2(const D2D1_POINT_2F& point);
	virtual bool OnMouseUpL2(const D2D1_POINT_2F& point);

	virtual bool OnMouseLeave(const D2D1_POINT_2F& point);
	virtual bool OnMouseEnter(const D2D1_POINT_2F& point);

	virtual bool OnMouseWheel(float delta);
	virtual bool OnKeyDown(DWORD keyCode);
	virtual bool OnKeyUp(DWORD keyCode);
	virtual bool OnChar(DWORD ch);
	virtual void OnDpiChanged(float dpiScaleX, float dpiScaleY);

protected:
	D2D1_RECT_F m_rectangle;
	float m_dpiScaleX;
	float m_dpiScaleY;
	void* m_userData;

	CD2DBorder m_border;
	CD2DBackground m_background;
	CD2DTransform m_transform;
};

// 实现 CD2DControlUI
inline CD2DControlUI::CD2DControlUI()
	: m_dpiScaleX(1.0f), m_dpiScaleY(1.0f), m_userData(nullptr)
{
	m_rectangle = D2D1::RectF(0.0f, 0.0f, 0.0f, 0.0f);
	SetRectangleInternal(m_rectangle);
}

inline void CD2DControlUI::SetPosition(float x, float y)
{
	float width = m_rectangle.right - m_rectangle.left;
	float height = m_rectangle.bottom - m_rectangle.top;
	m_rectangle = D2D1::RectF(x, y, x + width, y + height);
	SetRectangleInternal(m_rectangle);
}

inline void CD2DControlUI::SetSize(float width, float height)
{
	m_rectangle.right = m_rectangle.left + width;
	m_rectangle.bottom = m_rectangle.top + height;
	SetRectangleInternal(m_rectangle);
}

inline void CD2DControlUI::SetContentSize(float width, float height)
{
	// 调整为包含所有 margin, border, padding 的大小
	float totalWidth = width + GetMarginLeft() + GetMarginRight() + 
					   GetBorderLeft() + GetBorderRight() + 
					   GetPaddingLeft() + GetPaddingRight();
	float totalHeight = height + GetMarginTop() + GetMarginBottom() + 
					    GetBorderTop() + GetBorderBottom() + 
					    GetPaddingTop() + GetPaddingBottom();
	SetSize(totalWidth, totalHeight);
}

inline void CD2DControlUI::SetRectangle(const D2D1_RECT_F& rect)
{
	m_rectangle = rect;
	SetRectangleInternal(m_rectangle);
}

inline void CD2DControlUI::SetCenter(float x, float y)
{
	float width = m_rectangle.right - m_rectangle.left;
	float height = m_rectangle.bottom - m_rectangle.top;
	m_rectangle = D2D1::RectF(x - width / 2.0f, y - height / 2.0f, 
							  x + width / 2.0f, y + height / 2.0f);
	SetRectangleInternal(m_rectangle);
}

inline D2D1_RECT_F CD2DControlUI::GetRectangle() const
{
	return m_rectangle;
}

inline D2D1_ROUNDED_RECT CD2DControlUI::GetRoundedRectangle() const
{
	const D2D1_POINT_2F& topLeft = m_border.GetTopLeftRadius();
	return D2D1::RoundedRect(m_rectangle, topLeft.x, topLeft.y);
}

inline D2D1_POINT_2F CD2DControlUI::GetPosition() const
{
	return D2D1::Point2F(m_rectangle.left, m_rectangle.top);
}

inline D2D1_SIZE_F CD2DControlUI::GetSize() const
{
	return D2D1::SizeF(m_rectangle.right - m_rectangle.left, 
					   m_rectangle.bottom - m_rectangle.top);
}

inline D2D1_POINT_2F CD2DControlUI::GetCenter() const
{
	return D2D1::Point2F((m_rectangle.left + m_rectangle.right) / 2.0f,
						(m_rectangle.top + m_rectangle.bottom) / 2.0f);
}

inline float CD2DControlUI::GetWidth() const
{
	return m_rectangle.right - m_rectangle.left;
}

inline float CD2DControlUI::GetHeight() const
{
	return m_rectangle.bottom - m_rectangle.top;
}

inline void CD2DControlUI::Move(float offsetX, float offsetY)
{
	m_rectangle.left += offsetX;
	m_rectangle.right += offsetX;
	m_rectangle.top += offsetY;
	m_rectangle.bottom += offsetY;
	SetRectangleInternal(m_rectangle);
}

inline void CD2DControlUI::MoveTo(float x, float y)
{
	SetPosition(x, y);
}

inline void CD2DControlUI::Inflate(float dx, float dy)
{
	m_rectangle.left -= dx;
	m_rectangle.right += dx;
	m_rectangle.top -= dy;
	m_rectangle.bottom += dy;
	SetRectangleInternal(m_rectangle);
}

inline void CD2DControlUI::Resize(float newWidth, float newHeight)
{
	SetSize(newWidth, newHeight);
}

inline bool CD2DControlUI::IsHitTest(const D2D1_POINT_2F& point, const D2D1_RECT_F& rect) const
{
	return point.x >= rect.left && point.x <= rect.right &&
		   point.y >= rect.top && point.y <= rect.bottom;
}

inline bool CD2DControlUI::IsHitTest(const D2D1_POINT_2F& point) const
{
	return IsHitTest(point, m_rectangle);
}

inline bool CD2DControlUI::IsHitTest(float x, float y) const
{
	return IsHitTest(D2D1::Point2F(x, y));
}

inline void CD2DControlUI::SetUserData(void* userData)
{
	m_userData = userData;
}

inline void* CD2DControlUI::GetUserData() const
{
	return m_userData;
}

inline bool CD2DControlUI::OnMouseDownL(const D2D1_POINT_2F& point)
{
	if (IsHitTest(point))
	{
		SetPressed(true);
		return true;
	}
	return false;
}

inline bool CD2DControlUI::OnMouseMove(const D2D1_POINT_2F& point)
{
	return IsHitTest(point);
}

inline bool CD2DControlUI::OnMouseUpL(const D2D1_POINT_2F& point)
{
	SetPressed(false);
	return IsHitTest(point);
}

inline bool CD2DControlUI::OnMouseDownL2(const D2D1_POINT_2F& point)
{
	return IsHitTest(point);
}

inline bool CD2DControlUI::OnMouseUpL2(const D2D1_POINT_2F& point)
{
	return IsHitTest(point);
}

inline bool CD2DControlUI::OnMouseLeave(const D2D1_POINT_2F& point)
{
	SetHover(false);
	return true;
}

inline bool CD2DControlUI::OnMouseEnter(const D2D1_POINT_2F& point)
{
	SetHover(true);
	return true;
}

inline bool CD2DControlUI::OnMouseWheel(float delta)
{
	return false;
}

inline bool CD2DControlUI::OnKeyDown(DWORD keyCode)
{
	return false;
}

inline bool CD2DControlUI::OnKeyUp(DWORD keyCode)
{
	return false;
}

inline bool CD2DControlUI::OnChar(DWORD ch)
{
	return false;
}

inline void CD2DControlUI::OnDpiChanged(float dpiScaleX, float dpiScaleY)
{
	m_dpiScaleX = dpiScaleX;
	m_dpiScaleY = dpiScaleY;
}
