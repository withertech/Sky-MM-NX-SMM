#include "tab_mods.hpp"
std::string g_prev_mod;
std::string g_sel_mod;
bool g_dialog_open;
tab_mods::tab_mods()
{
	setModsTabPtr(this);
	this->triggerUpdateListItems = false;
	this->triggerUpdateModsDisplayedStatus = false;
	this->frameCounter = -1;
	this->updateListItems();
}
void tab_mods::updateListItems()
{
	this->clear();
	// Setup the list
	auto mod_folders_list = getGlobalModList();
	for (int i_folder = 0; i_folder < int(mod_folders_list.size()); i_folder++)
	{
		std::string selected_mod = mod_folders_list[i_folder].get()->base_name;
		brls::Logger::debug("Adding mod: %s", selected_mod.c_str());
		ModListItem *item = new ModListItem(selected_mod, "", "");
		item->getClickEvent()->subscribe([this, selected_mod](View *view) {
			if (!g_dialog_open)
			{
				auto *dialog = new brls::Dialog("sky/dialog/enable1"_i18n + selected_mod + "sky/dialog/enable2"_i18n);

				dialog->addButton("sky/dialog/yes"_i18n, [selected_mod, dialog, this](brls::View *view) {
					std::shared_ptr<SkyrimMod> mod = find_mod(getGlobalModList(), selected_mod);
					switch (mod->getStatus())
					{
					case ModStatus::ENABLED:
					{
						mod->enable();
						g_dialog_open = false;
						dialog->close();
						break;
					}
					case ModStatus::PARTIAL:
					{
						mod->enable();
						g_dialog_open = false;
						dialog->close();
						break;
					}
					case ModStatus::DISABLED:
					{
						mod->enable();
						g_dialog_open = false;
						dialog->close();
						break;
					}
					default:
						break;
					}
					this->triggerUpdateModsDisplayedStatus = true;
					g_dirty = true;
					clearTempEffects();
				});
				dialog->addButton("sky/dialog/no"_i18n, [dialog](brls::View *view) {
					g_dialog_open = false;
					dialog->close();
				});

				dialog->setCancelable(true);
				g_dialog_open = true;
				dialog->open();
			}

			return true;
		});
		item->updateActionHint(brls::Key::A, "sky/hints/enable"_i18n);

		item->registerAction("sky/hints/disable"_i18n, brls::Key::X, [this, selected_mod] {
			if (!g_dialog_open)
			{
				auto *dialog = new brls::Dialog("sky/dialog/disable1"_i18n + selected_mod + "sky/dialog/disable2"_i18n);

				dialog->addButton("sky/dialog/yes"_i18n, [selected_mod, dialog, this](brls::View *view) {
					std::shared_ptr<SkyrimMod> mod = find_mod(getGlobalModList(), selected_mod);
					switch (mod->getStatus())
					{
					case ModStatus::ENABLED:
					{
						mod->disable();
						g_dialog_open = false;
						dialog->close();
						break;
					}
					case ModStatus::PARTIAL:
					{
						mod->disable();
						g_dialog_open = false;
						dialog->close();
						break;
					}
					case ModStatus::DISABLED:
					{
						mod->disable();
						g_dialog_open = false;
						dialog->close();
						break;
					}
					default:
						break;
					}
					this->triggerUpdateModsDisplayedStatus = true;
					g_dirty = true;
					clearTempEffects();
				});
				dialog->addButton("sky/dialog/no"_i18n, [dialog](brls::View *view) {
					g_dialog_open = false;
					dialog->close();
				});

				dialog->setCancelable(true);
				g_dialog_open = true;
				dialog->open();
			}
			return true;
		});

		// item->setValue("Unchecked");
		// item->setValueActiveColor(nvgRGB(80, 80, 80));

		this->addView(item);
		_modsListItems_[selected_mod] = item;
		this->setTriggerUpdateModsDisplayedStatus(true);
	}

	if (mod_folders_list.empty())
	{

		auto *emptyListLabel = new brls::ListItem(
			"sky/msg/missing1"_i18n + getRomfsPath("Data"),
			"sky/msg/missing2"_i18n);
		emptyListLabel->show([]() {}, false);
		this->addView(emptyListLabel);
	}
	else
	{
		this->registerAction(
			"sky/hints/edit"_i18n, brls::Key::Y, [] {
				if (g_edit_load_order)
				{
					g_status_msg = "";
					g_edit_load_order = false;
				}
				else
				{
					g_status_msg = "sky/msg/edit"_i18n;
					g_edit_load_order = true;
				}
				return true;
			});
	}
}
std::map<std::string, ModListItem *> &tab_mods::getModsListItems()
{
	return _modsListItems_;
}
void tab_mods::onChildFocusLost(View *child)
{
	View::onChildFocusLost(child);
}

void tab_mods::onChildFocusGained(View *child)
{
	if (g_edit_load_order)
	{
		std::shared_ptr<SkyrimMod> old_mod = find_mod(getGlobalModList(), g_prev_mod);
		std::shared_ptr<SkyrimMod> new_mod = find_mod(getGlobalModList(), g_sel_mod);
		ModList::iterator old_i;
		ModList::iterator new_i;
		for (auto it = getGlobalModList().begin(); it != getGlobalModList().end(); it++)
		{
			if ((*it)->base_name == old_mod->base_name)
			{
				old_i = it;
			}
			if ((*it)->base_name == new_mod->base_name)
			{
				new_i = it;
			}
		}
		std::iter_swap(old_i, new_i);
		g_dirty = true;
		this->setTriggerUpdateListItems(true);
		View::onChildFocusGained(child);
		g_edit_load_order = false;
		g_status_msg = "";
	}
}
void tab_mods::draw(NVGcontext *vg, int x, int y, unsigned int width, unsigned int height, brls::Style *style,
					brls::FrameContext *ctx)
{

	ScrollView::draw(vg, x, y, width, height, style, ctx);

	if (this->triggerUpdateModsDisplayedStatus)
	{
		this->updateDisplayedModsStatus();
		this->triggerUpdateModsDisplayedStatus = false;
	}

	if (this->triggerUpdateListItems)
	{
		this->updateListItems();
		brls::Application::giveFocus(getModsListItems()[g_prev_mod]);
		this->triggerUpdateListItems = false;
	}
}

void tab_mods::setTriggerUpdateModsDisplayedStatus(bool triggerUpdateModsDisplayedStatus_)
{
	tab_mods::triggerUpdateModsDisplayedStatus = triggerUpdateModsDisplayedStatus_;
}

void tab_mods::setTriggerUpdateListItems(bool triggerUpdateListItems_)
{
	tab_mods::triggerUpdateListItems = triggerUpdateListItems_;
}

void tab_mods::updateDisplayedModsStatus()
{
	for (auto &modItem : _modsListItems_)
	{
		std::string reference_str = modItem.first;
		std::shared_ptr<SkyrimMod> mod = find_mod(getGlobalModList(), reference_str);
		NVGcolor color;
		switch (mod->getStatus())
		{
		case ModStatus::ENABLED:
			modItem.second->setValue("sky/status/enabled"_i18n);
			color = nvgRGB(88, 195, 169);
			break;
		case ModStatus::PARTIAL:
			modItem.second->setValue("sky/status/partial"_i18n);
			color = nvgRGB(245 * 0.85, 198 * 0.85, 59 * 0.85);
			break;
		case ModStatus::DISABLED:
			modItem.second->setValue("sky/status/disabled"_i18n);
			color = nvgRGB(80, 80, 80);
			break;
		}

		this->_modsListItems_[modItem.first]->setValueActiveColor(color);
	}
}