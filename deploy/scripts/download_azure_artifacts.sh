# It needs the --run-id. This can be obtained from the URL of the azure-pipelines build
# https://dev.azure.com/phcerdan/SGEXT/_build/results?buildId=181&
set -x
if [ $# -eq 0 ]; then
    echo "No argument provided, provide run-id."
    exit 1
fi
run_id=$1
base_command="\
  az pipelines runs artifact download \
  --organization=https://dev.azure.com/phcerdan \
  --project sgext \
  --run-id ${run_id}"
artifact_names=("MacOSWheel3.6" "MacOSWheel3.7" "MacOSWheel3.8" "MacOSWheel3.9" "WindowsWheel3.6" "WindowsWheel3.7" "WindowsWheel3.8" "WindowsWheel3.9")
# Linux
linux_wheels_name=LinuxWheels
linux_command="$base_command --path /tmp --artifact-name ${linux_wheels_name}"
eval $linux_command
for name in ${artifact_names[*]}; do
  com="$base_command --path /tmp/dist --artifact-name ${name}"
  eval $com
done
