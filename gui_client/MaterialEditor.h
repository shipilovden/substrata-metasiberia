#pragma once


#include "../shared/WorldMaterial.h"
#include "../utils/IncludeWindows.h" // This needs to go first for NOMINMAX.
#include "../opengl/OpenGLEngine.h"
#include "../maths/vec2.h"
#include "../maths/vec3.h"
#include "../utils/Timer.h"
#include "../utils/Reference.h"
#include "../utils/RefCounted.h"
#include "../graphics/colour3.h"
#include "ui_MaterialEditor.h"
#include <QtCore/QEvent>
#include <map>


namespace Indigo { class Mesh; }
class TextureServer;
class EnvEmitter;


class MaterialEditor : public QWidget, public Ui::MaterialEditor
{
	Q_OBJECT        // must include this if you use Qt signals/slots

public:
	MaterialEditor(QWidget *parent = 0);
	~MaterialEditor();

	void setFromMaterial(const WorldMaterial& mat);
	void toMaterial(WorldMaterial& mat_out);

	void setControlsEnabled(bool enabled);

	void setControlsEditable(bool editable);

	// Call to apply translations to all UI strings of this widget
	void retranslate() { Ui::MaterialEditor::retranslateUi(this); }
protected:
	void updateColourButton();
	void updateEmissionColourButton();

signals:;
	void materialChanged();

protected slots:
	void on_colourPushButton_clicked(bool checked);
	void on_emissionColourPushButton_clicked(bool checked);
	
private:
	Colour3f col;
	Colour3f emission_col;
};
