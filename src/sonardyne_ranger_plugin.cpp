#include "rqt_sonardyne/sonardyne_ranger_plugin.h"
#include <pluginlib/class_list_macros.h>
#include <ros/master.h>
#include <marine_msgs/KeyValue.h>
#include <QLineEdit>
#include <QLabel>

namespace rqt_sonardyne
{
    
SonardyneRangerPlugin::SonardyneRangerPlugin():rqt_gui_cpp::Plugin()
{
  setObjectName("SonardyneRanger");
}
 
void SonardyneRangerPlugin::initPlugin(qt_gui_cpp::PluginContext& context)
{
  widget_ = new QWidget();
  ui_.setupUi(widget_);
  
  widget_->setWindowTitle(widget_->windowTitle() + " (" + QString::number(context.serialNumber()) + ")");
  
  context.addWidget(widget_);
  
  updateTopicList();
  ui_.topicsComboBox->setCurrentIndex(ui_.topicsComboBox->findText(""));
  connect(ui_.topicsComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onTopicChanged(int)));

  ui_.refreshTopicsPushButton->setIcon(QIcon::fromTheme("view-refresh"));
  connect(ui_.refreshTopicsPushButton, SIGNAL(pressed()), this, SLOT(updateTopicList()));
  
  // set topic name if passed in as argument
  const QStringList& argv = context.argv();
  if (!argv.empty()) {
      arg_topic_ = argv[0];
      selectTopic(arg_topic_);
  }
}

void SonardyneRangerPlugin::shutdownPlugin()
{
  device_status_subscriber_.shutdown();
}

void SonardyneRangerPlugin::saveSettings(qt_gui_cpp::Settings& plugin_settings, qt_gui_cpp::Settings& instance_settings) const
{
  QString topic = ui_.topicsComboBox->currentText();
  instance_settings.setValue("topic", topic);
}

void SonardyneRangerPlugin::restoreSettings(const qt_gui_cpp::Settings& plugin_settings, const qt_gui_cpp::Settings& instance_settings)
{
  QString topic = instance_settings.value("topic", "").toString();
  // don't overwrite topic name passed as command line argument
  if (!arg_topic_.isEmpty())
  {
    arg_topic_ = "";
  }
  else
  {
    selectTopic(topic);
  }
}

void SonardyneRangerPlugin::updateTopicList()
{
  QString selected = ui_.topicsComboBox->currentText();
  ros::master::V_TopicInfo topic_info;
  ros::master::getTopics(topic_info);
    
  QList<QString> topics;
  for(const auto t: topic_info)
    if (t.datatype == "sonardyne_msgs/DeviceStatus")
      topics.append(t.name.c_str());
        
  topics.append("");
  qSort(topics);
  ui_.topicsComboBox->clear();
  for (QList<QString>::const_iterator it = topics.begin(); it != topics.end(); it++)
  {
    QString label(*it);
    label.replace(" ", "/");
    ui_.topicsComboBox->addItem(label, QVariant(*it));
  }

  // restore previous selection
  selectTopic(selected);
}

void SonardyneRangerPlugin::selectTopic(const QString& topic)
{
  int index = ui_.topicsComboBox->findText(topic);
  if (index == -1)
  {
    // add topic name to list if not yet in
    QString label(topic);
    label.replace(" ", "/");
    ui_.topicsComboBox->addItem(label, QVariant(topic));
    index = ui_.topicsComboBox->findText(topic);
  }
  ui_.topicsComboBox->setCurrentIndex(index);
}

void SonardyneRangerPlugin::onTopicChanged(int index)
{
  device_status_subscriber_.shutdown();
    
  QString topic = ui_.topicsComboBox->itemData(index).toString();
  if(!topic.isEmpty())
  {
    device_status_subscriber_ = getNodeHandle().subscribe(topic.toStdString(), 10, &SonardyneRangerPlugin::deviceStatusCallback, this);
  }
}

void SonardyneRangerPlugin::deviceStatusCallback(const sonardyne_msgs::DeviceStatus& msg)
{
  std::string label = msg.name+" "+msg.type+" "+msg.UID+" state: "+std::to_string(msg.state);
  if(device_map_.find(msg.UID) == device_map_.end())
  {
    device_map_[msg.UID] = new QListWidgetItem(label.c_str());
    ui_.deviceStatusListWidget->addItem(device_map_[msg.UID]);
  }
  else
    device_map_[msg.UID]->setData(Qt::DisplayRole, QString(label.c_str()));
}


} // namespace rqt_sonardyne

PLUGINLIB_EXPORT_CLASS(rqt_sonardyne::SonardyneRangerPlugin, rqt_gui_cpp::Plugin)
