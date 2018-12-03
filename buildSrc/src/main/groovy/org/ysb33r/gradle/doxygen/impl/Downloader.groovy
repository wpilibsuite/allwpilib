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
import org.gradle.api.Project
import org.gradle.util.GradleVersion
import org.ysb33r.grolifant.api.AbstractDistributionInstaller
import org.ysb33r.grolifant.api.OperatingSystem
import org.ysb33r.grolifant.api.errors.DistributionFailedException

/** Downloads specific versions of Doxygen.
 * Curretnly limited to Linux, Windows & MacOS X on x86 32 + 64-bit architectures as these are the only ones for which
 * binary packages are available from the Doxygen site.
 */
@CompileStatic
class Downloader extends AbstractDistributionInstaller {
    static final OperatingSystem OS = OperatingSystem.current()

    String baseURI = 'https://sourceforge.net/projects/doxygen/files'

    // https://sourceforge.net/projects/doxygen/files/rel-1.8.14/doxygen-1.8.14.windows.bin.zip/download


    Downloader(final String version,final Project project) {
        super('doxygen',version,'native-binaries/doxygen',project)
    }

    /** Provides an appropriate URI to download a specific verson of Doxygen.
     *
     * @param ver Version of Doxygen to download
     * @return URI for Linux, WIndows (32,64 bit) or MacOSX. {@code null} otherwise
     */
    @Override
    URI uriFromVersion(final String ver) {
        if(OS.isWindows()) {
            // Using GradleVersion as it has a handy version comparison
            if(GradleVersion.version(ver) >= GradleVersion.version('1.8.0') && System.getProperty('os.arch').contains('64')) {
                "${baseURI}/rel-${ver}/doxygen-${ver}.windows.x64.bin.zip".toURI()
            } else {
                "${baseURI}/rel-${ver}/doxygen-${ver}.windows.bin.zip".toURI()
            }
        } else if(OS.isLinux()) {
            "${baseURI}/rel-${ver}/doxygen-${ver}.linux.bin.tar.gz".toURI()
        } else if(OS.isMacOsX()) {
            "${baseURI}/rel-${ver}/Doxygen-${ver}.dmg".toURI()
        } else {
            null
        }
    }

    /** Adds a special verification case for Doxygen Windows binaries.
     *
     * The Windows distribution is not zipped into a subfolder and needs extra care.
     * For Linux & MacOsX it will use the default implementation.
     *
     * @param distDir Directory where distribution was unpacked to.
     * @param distributionDescription A human-readable description
     * @return Distribution directory
     */
    @Override
    protected File getAndVerifyDistributionRoot(File distDir, String distributionDescription) {
        if(OS.windows) {
            if (!new File(distDir,'doxygen.exe').exists()) {
                throw new DistributionFailedException("Doxygen '${distributionDescription}' does not contain 'doxygen.exe'.")
            }
            return distDir
        } else {
            super.getAndVerifyDistributionRoot(distDir, distributionDescription)
        }
    }

    /** Returns the path to the Doxygen executable.
     * Will force a download if not already downloaded.
     *
     * @return Location of {@code doxygen} or null if not a supported operating system.
     */
    File getDoxygenExecutablePath() {
        if(OS.isWindows()) {
            new File(distributionRoot,'doxygen.exe')
        } else if(OS.isLinux()) {
            new File(distributionRoot,'bin/doxygen')
        } else if(OS.isMacOsX()) {
            new File(distributionRoot,'Contents/Resources/doxygen')
        } else {
            null
        }

    }

    /** Unpacks a Doxygen archive.
     * For {@code zip} and {@code tar.gz} formats the standard operations will be used.
     * For {@code dmg} formats will call out to {@code hdiutil} to temporary mount/unmount image.
     *
     * @param srcArchive Location of downloaded archive.
     * @param destDir Directory to unpack to.
     */
    @Override
    protected void unpack(final File srcArchive, final File destDir) {

        if(OS.macOsX && srcArchive.name.endsWith('.dmg')) {
            unpackDmgOnMacOsX(srcArchive,destDir)
        } else {
            super.unpack(srcArchive,destDir)
        }
    }

    @CompileDynamic
    private void unpackDmgOnMacOsX(final File srcArchive, final File destDir) {
        final File mountRoot = File.createTempDir('gradle_doxygen_tmp','$$$')
        final File mountedPath = new File(mountRoot,srcArchive.name)
        mountedPath.mkdirs()
        getProject().exec {
            executable 'hdiutil'
            args 'attach', srcArchive.absolutePath, '-nobrowse', '-readonly'
            args '-mountpoint', mountedPath.absolutePath
        }
        try {
            getProject().copy {
                from "${mountedPath}/Doxygen.app", {
                    include '**'
                }
                into "${destDir}/Doxygen.app"
            }
        } finally {
            getProject().exec() {
                executable 'hdiutil'
                args 'detach', mountedPath.absolutePath
                ignoreExitValue = true
            }
            mountedPath.deleteDir()
        }
    }
}

