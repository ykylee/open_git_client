#include "app/application_bootstrap.h"

namespace ogc::app {

ApplicationBootstrap::ApplicationBootstrap()
    : repositoryUseCases_(gitBackend_) {
}

application::RepositoryUseCases& ApplicationBootstrap::repositoryUseCases() {
    return repositoryUseCases_;
}

}  // namespace ogc::app
