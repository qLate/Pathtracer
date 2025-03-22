#pragma once

class Object;
class Graphical;

class BaseInspectorDrawer
{
protected:
	virtual void drawInnerWrapper(Object* target) = 0;

public:
	virtual void draw(Object* target);
};

class ObjectInspectorDrawer : public BaseInspectorDrawer
{
	static void drawInner(Object* target);

protected:
	void drawInnerWrapper(Object* target) override;
};

class GraphicalInspectorDrawer : public ObjectInspectorDrawer
{
	static void drawInner(Graphical* target);

protected:
	void drawInnerWrapper(Object* target) override;
};
