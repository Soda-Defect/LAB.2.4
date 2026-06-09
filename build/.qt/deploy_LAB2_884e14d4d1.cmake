include("D:/LABS/LAB.2.2/build/.qt/QtDeploySupport.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/LAB2-plugins.cmake" OPTIONAL)
set(__QT_DEPLOY_I18N_CATALOGS "qtbase")

qt6_deploy_runtime_dependencies(
    EXECUTABLE "D:/LABS/LAB.2.2/build/LAB2.exe"
    GENERATE_QT_CONF
)
