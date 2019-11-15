pipeline{
    agent { label "win32"}

    parameters {
        string(name: 'BUILD_MODE', defaultValue: 'Release', description: 'build type(Release or Debug)')
        booleanParam(name: 'VCS_SKIP_UPLOAD', defaultValue: false, description: "skip upload steps?")
    }

    environment {
        GIT_SHOT_HASH_ID=env.GIT_COMMIT.take(8)
		SEC_ENV_WIN32=credentials('SEC_ENV_WIN32')
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
        stage("Archive"){
            steps{
                bat "7z a ${params.BUILD_MODE}.7z build/${params.BUILD_MODE}"
            }
        }
        stage("Upload VCS"){
            steps{
                bat "echo ${BUILD_TAG} %date% %time% > BUILD_INFO.txt"
                bat "cmd \\V \\C \" ${SEC_ENV_WIN32} && vcsup %vcsUrl% -r qvcs_gui_win32 -v ${BUILD_NUMBER}_${GIT_SHOT_HASH_ID} -f ${params.BUILD_MODE}.7z -m BUILD_INFO.txt\""
            }
        }
    }
    post{
        always{
            cleanWs()
        }
    }
}