// Copyright (C) 2020  I. Bogoslavskyi, C. Stachniss
//
// GNU-GPL licence that follows one of libQGLViewer.

#ifndef SRC_VISUALIZATION_VISUALIZER_H_
#define SRC_VISUALIZATION_VISUALIZER_H_

#include <QGLViewer/qglviewer.h>

#include <map>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>
#include <utility>

#include "communication/abstract_client.h"
#include "communication/abstract_sender.h"
#include "utils/cloud.h"
#include "utils/useful_typedefs.h"

namespace depth_clustering {

class IUpdateListener {
 public:
  virtual void onUpdate() = 0;
};

class ObjectPtrStorer
    : public AbstractClient<std::unordered_map<uint16_t, Cloud>> {
 public:
  ObjectPtrStorer() : AbstractClient<std::unordered_map<uint16_t, Cloud>>() {}

  void OnNewObjectReceived(const std::unordered_map<uint16_t, Cloud>& clouds,
                           const int id) override;

  void SetUpdateListener(IUpdateListener* update_listener) {
    _update_listener = update_listener;
  }

  virtual ~ObjectPtrStorer() {}

  std::unordered_map<uint16_t, Cloud> object_clouds() const;

 private:
  std::unordered_map<uint16_t, Cloud> _obj_clouds;
  IUpdateListener* _update_listener;
  mutable std::mutex _cluster_mutex;
};

/**
 * @brief      An OpenGl visualizer that shows data that is subscribes to.
 */
class Visualizer : public QGLViewer,
                   public AbstractClient<Cloud>,
                   public AbstractSender<std::pair<char,float> >,
                   public IUpdateListener {

  using SenderT = AbstractSender<std::pair<char,float> >;

 public:
  explicit Visualizer(QWidget* parent = 0);
  virtual ~Visualizer();

  void OnNewObjectReceived(const Cloud& cloud, const int id) override;

  void onUpdate() override;

  std::pair<char,float> getClusters();

  ObjectPtrStorer* object_clouds_client() { return &_cloud_obj_storer; }

 protected:
  void draw() override;
  void init() override;

 private:
  void DrawCloud(const Cloud& cloud);
  void DrawCloudColored(const Cloud& cloud, float r, float g, float b);
  void DrawCube(const Eigen::Vector3f& center, const Eigen::Vector3f& scale);

  bool _updated;
  ObjectPtrStorer _cloud_obj_storer;
  Cloud _cloud;
  std::vector<std::pair<Eigen::Vector3f, Eigen::Vector3f> > myclusters;
  mutable std::mutex _cloud_mutex;
};

}  // namespace depth_clustering

#endif  // SRC_VISUALIZATION_VISUALIZER_H_
