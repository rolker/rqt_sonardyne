#ifndef RQT_SONARDYNE_SONARDYNE_RANGER_PLUGIN_H
#define RQT_SONARDYNE_SONARDYNE_RANGER_PLUGIN_H

#include <rqt_gui_cpp/plugin.h>
#include <ui_sonardyne_ranger_plugin.h>
#include <ros/ros.h>
#include <sonardyne_msgs/DeviceStatus.h>

class QLabel;

namespace rqt_sonardyne
{
    
class SonardyneRangerPlugin: public rqt_gui_cpp::Plugin
{
  Q_OBJECT
public:
  SonardyneRangerPlugin();
    
  void initPlugin(qt_gui_cpp::PluginContext& context) override;
  void shutdownPlugin() override;
  void saveSettings(qt_gui_cpp::Settings& plugin_settings, qt_gui_cpp::Settings& instance_settings) const override;
  void restoreSettings(const qt_gui_cpp::Settings& plugin_settings, const qt_gui_cpp::Settings& instance_settings) override;
    
protected slots:
  void updateTopicList();
    
  void selectTopic(const QString& topic);

  void onTopicChanged(int index);
  
  void deviceStatusCallback(const sonardyne_msgs::DeviceStatus &msg);

private:
  Ui::SonardyneRangerWidget ui_;
  QWidget* widget_ = nullptr;
  ros::Subscriber device_status_subscriber_;

  std::map<std::string, QListWidgetItem*> device_map_;

  QString arg_topic_;
};

} // namespace rqt_marine_radar

#endif
