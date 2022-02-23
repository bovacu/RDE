GDE_PATH="$1"
PROJECT_NAME="$2"
PROJECT_PATH="$3"


cp -r projectBase/TestProjectEngine "$PROJECT_PATH"
mv "$PROJECT_PATH"/TestProjectEngine "$PROJECT_PATH"/"$PROJECT_NAME"

cd "$PROJECT_PATH"/"$PROJECT_NAME"
sed -i -e "s|PLACEHOLDER|$PROJECT_NAME|g" CMakeLists.txt
sed -i -e "s|set(GDE_PATH)|set(GDE_PATH $GDE_PATH)|g" CMakeLists.txt