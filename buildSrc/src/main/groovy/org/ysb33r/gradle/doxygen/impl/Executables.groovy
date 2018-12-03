//
// ============================================================================
// (C) Copyright Schalk W. Cronje 2013-2017
//
// This software is licensed under the Apache License 2.0
// See http://www.apache.org/licenses/LICENSE-2.0 for license details
//
// Unless required by applicable law or agreed to in writing, software distributed under the License is
// distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and limitations under the License.
//
// ============================================================================
//

package org.ysb33r.gradle.doxygen.impl

import groovy.transform.CompileDynamic
import groovy.transform.CompileStatic
import org.gradle.api.GradleException
import org.gradle.api.Project
import org.gradle.internal.os.OperatingSystem
import org.gradle.util.CollectionUtils
import org.ysb33r.grolifant.api.StringUtils
import org.ysb33r.grolifant.api.UriUtils

/**
 * Created by schalkc on 23/05/2014.
 */
@CompileStatic
class Executables {

    static final Map<String,String> EXECUTABLES = [
            'mscgen'  : 'MSCGEN_PATH',
            'dot'     : 'DOT_PATH',
            'perl'    : 'PERL_PATH',
            'hhc'     : 'HHC_LOCATION'
    ]

    Executables(Map<String,Object> map, Project project ) {
        final OperatingSystem OS = OperatingSystem.current()
        this.mapToUpdate = map
        this.project = project
        this.versionSupportedOnOS = OS.isWindows() || OS.isLinux() || OS.isMacOsX()
    }

    /** Set the doxygen executable to use. This can be in the form of a version or a path.
     * The version variant is supported on Linux, MacOSX and Windows. On any other operating system only
     * the path version will be supported at this point due to the fact that these are the only binary formats
     * distributed by the Doxygen project.
     *
     * @param Map taking either {@code path} or {@code version} as key. if the latter is used then the following
     * keys may also be used.
     * <li> {@code baseURI} Override the base URI to download Doxygen binary distributions from.
     * <li> {@code downloadRoot} Override the default location underneath {@code GRADLE_USER_HOME}. Primarily used for testing purposes.
     *
     * Any other keys are ignored.
     *
     * @since 0.3
     */
    void doxygen(final Map<String,Object> param) {

        if(param.containsKey('version') && param.containsKey('path')) {
            throw new GradleException("Cannot use both 'version' and 'path' keywords for setting Doxygen executable")
        }

        if(param.containsKey('version')) {
            if(versionSupportedOnOS) {
                mapToUpdate['doxygen'] = {
                    Downloader dwnl = new Downloader(StringUtils.stringize(param['version']), project)

                    if(param.containsKey('baseURI')) {
                        dwnl.baseURI = UriUtils.urize(param['baseURI'])
                    }

                    if(param.containsKey('downloadRoot')) {
                        dwnl.downloadRoot = project.file(param['downloadRoot'])
                    }

                    dwnl.doxygenExecutablePath.absolutePath
                }
            } else {
                throw new GradleException('The \'version\' keyword is not supported on this operating system')
            }
        }

        if(param.containsKey('path')) {
            mapToUpdate['doxygen'] = {CollectionUtils.stringize([param['path']])[0]}
        }

    }

    @CompileDynamic
    def methodMissing( String name, args ) {

        if( args.size() == 1 && EXECUTABLES[name] != null ) {
            switch (args[0]) {
                case File:
                    return mapToUpdate[name] = args[0].absolutePath
                default:
                    return mapToUpdate[name] = args[0].toString()
            }
        }

        throw new MissingMethodException(name,Executables.class,args)
    }

    private Map<String,Object> mapToUpdate
    private Project project
    private final boolean versionSupportedOnOS

}
