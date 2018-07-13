bazel build -c opt applications:trading_system

sudo cp bazel-bin/applications/trading_system docker/trading_system/
sudo cp bazel-bin/applications/trading_system release/

cp config/system_config_test docker/trading_system/
cp config/system_config_test release/
