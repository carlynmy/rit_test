#include "controller/controller.h"
#include "model/model.h"
#include "view/view.h"

int main(int argc, char** argv) {
  RitTest::Client::Model model;
  RitTest::Client::Controller controller(&model);
  RitTest::Client::View view(&controller);

  return view.Exec(argc, argv);
}
