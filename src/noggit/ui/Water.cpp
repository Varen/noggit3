// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#include <noggit/ui/Water.h>

#include <algorithm>
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include <noggit/Environment.h>
#include <noggit/application.h> // fonts
#include <noggit/ui/Text.h>
#include <noggit/ui/Button.h>
#include <noggit/ui/CheckBox.h>
#include <noggit/ui/Texture.h>
#include <noggit/ui/Slider.h>
#include <noggit/ui/MapViewGUI.h>
#include <noggit/Misc.h>
#include <noggit/World.h>
#include <noggit/Video.h> // video
#include <iostream>
#include <sstream>
#include <noggit/ui/WaterTypeBrowser.h>

#include <noggit/Log.h>

UIWater::UIWater(UIMapViewGUI *setGui)
  : UIWindow((float)video.xres() / 2.0f - (float)winWidth / 2.0f, (float)video.yres() / 2.0f - (float)winHeight / 2.0f - (float)(video.yres() / 4), (float)winWidth, (float)winHeight)
  , mainGui(setGui)
  , tile(0, 0)
  , _liquid_id(2)
  , _radius(10.0f)
  , _angle(10.0f)
  , _orientation(0.0f)
  , _locked(false)
  , _angled_mode(false)
  , _lock_pos(math::vector_3d(0.0f, 0.0f, 0.0f))
{
  addChild(new UIText(78.5f, 2.0f, "Water edit", app.getArial14(), eJustifyCenter));

  _radius_slider = new UISlider(5.0f, 35.0f, 170.0f, 250.0f, 0.0f);
  _radius_slider->setFunc([&](float f) { _radius = f;});
  _radius_slider->setValue(_radius / 250.0f);
  _radius_slider->setText("Radius:");
  addChild(_radius_slider);

  _angle_slider = new UISlider(6.0f, 65.0f, 167.0f, 89.0f, 0.00001f);
  _angle_slider->setFunc([&](float f) { _angle = f; });
  _angle_slider->setValue(_angle / 89.0f);
  _angle_slider->setText("Angle: ");
  addChild(_angle_slider);

  _orientation_slider = new UISlider(6.0f, 95.0f, 167.0f, 360.0f, 0.00001f);
  _orientation_slider->setFunc([&](float f) { _orientation = f; });
  _orientation_slider->setValue(_orientation / 360.0f);
  _orientation_slider->setText("Orientation: ");
  addChild(_orientation_slider);

  addChild(_angle_checkbox = new UICheckBox(5.0f, 110.0f, "Angled water", &_angled_mode));
  addChild(_lock_checkbox = new UICheckBox(5.0f, 135.0f, "Lock position", &_locked));

  waterType = new UIButton(5.0f, 170.0f, 170.0f, 30.0f,
    "Type: none",
    "Interface\\BUTTONS\\UI-DialogBox-Button-Disabled.blp",
    "Interface\\BUTTONS\\UI-DialogBox-Button-Down.blp",
    [this] { mainGui->guiWaterTypeSelector->toggleVisibility(); }
    );

  addChild(waterType);

  addChild(new UIText(5.0f, 200.0f, "Auto transparency:", app.getArial12(), eJustifyLeft));
  addChild(new UIButton(5.0f, 220.0f, 75.0f, 30.0f,
    "River",
    "Interface\\BUTTONS\\UI-DialogBox-Button-Disabled.blp",
    "Interface\\BUTTONS\\UI-DialogBox-Button-Down.blp",
    [] { gWorld->autoGenWaterTrans(0.0337f); } // value found by experimenting
  ));
  addChild(new UIButton(95.0f, 220.0f, 75.0f, 30.0f,
    "Ocean",
    "Interface\\BUTTONS\\UI-DialogBox-Button-Disabled.blp",
    "Interface\\BUTTONS\\UI-DialogBox-Button-Down.blp",
    [] { gWorld->autoGenWaterTrans(0.007f); }
  ));

  cropWater = new UIButton(5.0f, 250.0f, 170.0f, 30.0f,
    "Crop water",
    "Interface\\BUTTONS\\UI-DialogBox-Button-Disabled.blp",
    "Interface\\BUTTONS\\UI-DialogBox-Button-Down.blp",
    [this]
    {
      gWorld->CropWaterADT(gWorld->camera);
      updateData();
    }
    );
  addChild(cropWater);

  addChild(new UICheckBox(5.0f, 270.0f, "Show all layers", &Environment::getInstance()->displayAllWaterLayers));

  UIText *txt = new UIText(5.0f, 300.0f, app.getArial12(), eJustifyLeft);
  txt->setText("Current layer:");
  addChild(txt);


  waterLayer = new UIText(90.0f, 322.0f, app.getArial12(), eJustifyCenter);
  waterLayer->setText(std::to_string(Environment::getInstance()->currentWaterLayer + 1));
  addChild(waterLayer);

  addChild(new UIButton(28.0f, 320.0f, 50.0f, 30.0f,
    "<<",
    "Interface\\BUTTONS\\UI-DialogBox-Button-Disabled.blp",
    "Interface\\BUTTONS\\UI-DialogBox-Button-Down.blp",
    [this]
    {
      size_t layer = std::max(0, Environment::getInstance()->currentWaterLayer - 1);
      waterLayer->setText(std::to_string(layer + 1));
      Environment::getInstance()->currentWaterLayer = layer;
    }
    ));

  addChild(new UIButton(102.0f, 320.0f, 50.0f, 30.0f,
    ">>",
    "Interface\\BUTTONS\\UI-DialogBox-Button-Disabled.blp",
    "Interface\\BUTTONS\\UI-DialogBox-Button-Down.blp",
    [this]
    {
      size_t layer = std::min(4, Environment::getInstance()->currentWaterLayer + 1);
      waterLayer->setText(std::to_string(layer + 1));
      Environment::getInstance()->currentWaterLayer = layer;
    })
  );

  updateData();
}

void UIWater::updatePos(tile_index const& newTile)
{
  if (newTile == tile) return;

  tile = newTile;

  updateData();
}

void UIWater::updateData()
{
  std::stringstream mt;
  mt << _liquid_id << " - " << LiquidTypeDB::getLiquidName(_liquid_id);
  waterType->setText(mt.str());
}

void UIWater::changeWaterType(int waterint)
{
  _liquid_id = waterint;
  updateData();
}

void UIWater::changeRadius(float change)
{
  _radius = std::max(0.0f, std::min(250.0f, _radius + change));
  _radius_slider->setValue(_radius / 250.0f);
}

void UIWater::changeOrientation(float change)
{
  _orientation += change;

  if (_orientation < 0.0f)
  {
    _orientation += 360.0f;
  }
  else if (_orientation > 360.0f)
  {
    _orientation -= 360.0f;
  }

  _orientation_slider->setValue(_orientation / 360.0f);
}

void UIWater::changeAngle(float change)
{
  _angle = std::max(0.0f, std::min(89.0f, _angle + change));
  _angle_slider->setValue(_angle / 90.0f);
}

void UIWater::paintLiquid(math::vector_3d const& pos, bool add)
{
  if (_angled_mode)
  {
    gWorld->paintLiquid(pos, _radius, _liquid_id, add, math::degrees(_angle), math::degrees(_orientation), _locked, _lock_pos);
  }
  else
  {
    gWorld->paintLiquid(pos, _radius, _liquid_id, add, math::radians(0.0f), math::radians(0.0f), _locked, _lock_pos);
  }
  
}

void UIWater::toggle_lock()
{
  _locked = !_locked;
  _lock_checkbox->setState(_locked);
}

void UIWater::toggle_angled_mode()
{
  _angled_mode = !_angled_mode;
  _angle_checkbox->setState(_angled_mode);
}
