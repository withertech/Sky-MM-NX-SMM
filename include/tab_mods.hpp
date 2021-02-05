#pragma once

#include <borealis.hpp>
#include "mod.hpp"
#include "main.hpp"
#include "global.hpp"
#include <item_mod.hpp>
class ModListItem;
class tab_mods : public brls::List
{

public:
	tab_mods();
	std::map<std::string, ModListItem *> &getModsListItems();

	void setTriggerUpdateModsDisplayedStatus(bool triggerUpdateModsDisplayedStatus_);
	void setTriggerUpdateListItems(bool triggerUpdateListItems_);
	void updateDisplayedModsStatus();
	void updateListItems();
	void draw(NVGcontext *vg, int x, int y, unsigned width, unsigned height, brls::Style *style, brls::FrameContext *ctx) override;

private:
	brls::Dialog *dialog;
	std::map<std::string, ModListItem *> _modsListItems_;
	bool triggerUpdateModsDisplayedStatus;
	bool triggerUpdateListItems;
	int frameCounter;
	void onChildFocusLost(View *child);
	void onChildFocusGained(View *child);
};