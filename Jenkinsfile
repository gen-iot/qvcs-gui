pipeline{
    agent { label "win32"}

    parameters {
        string(name: 'BUILD_MODE', defaultValue: 'Release', description: 'build type(Release or Debug)')
        booleanParam(name: 'SKIP_UPLOAD', defaultValue: false, description: 'skip upload stage?')
    }

    environment {
        GIT_SHOT_HASH_ID=env.GIT_COMMIT.take(8)
    }

    stages{
        stage("Prepare"){
            steps{
                bat "set"
                echo "git short id=${GIT_SHOT_HASH_ID}"
                bat "pip3 install -U vcs-uploader"
            }
        }
        stage("CMake"){
            steps{
                dir("build"){
                    bat "cmake .."
                }
            }
        }
        stage("MSBuild"){
            steps{
                bat "${MS_BUILD} build/ALL_BUILD.vcxproj /p:Configuration=${params.BUILD_MODE}"
            }
        }
        stage("Upload VCS"){
            steps{
                echo "fake upload vcs"
                //bat "vcsup -r qvcs_gui_win32 -v ${} -h ${params.BUILD_MODE}.zip"
            }
        }
    }
    post{
        always{
            cleanWs()
        }
    }
}