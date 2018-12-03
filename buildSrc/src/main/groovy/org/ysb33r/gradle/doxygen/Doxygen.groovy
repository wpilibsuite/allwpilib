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

package org.ysb33r.gradle.doxygen

import groovy.transform.CompileDynamic
import groovy.transform.CompileStatic
import org.gradle.api.DefaultTask
import org.gradle.api.file.FileCollection
import org.gradle.api.tasks.InputFile
import org.gradle.api.tasks.SourceTask
import org.gradle.api.tasks.TaskAction
import org.gradle.api.tasks.Input
import org.gradle.api.tasks.Optional
import org.gradle.api.tasks.OutputDirectory
import org.gradle.process.ExecResult
import org.ysb33r.gradle.doxygen.impl.DoxyfileEditor
import org.ysb33r.gradle.doxygen.impl.DoxygenProperties
import org.ysb33r.gradle.doxygen.impl.Executables

/**
 * A wrapper task for calling GNU Make. This is useful for migrating legacy builds
 * or when complex build need to construct components that use GNU Make as a
 * build tool.
 *
 * @author Schalk W. Cronjé
 */
@CompileStatic
class Doxygen extends SourceTask {

    /** Docuemtnation output directory
     *
     * @return Directory that was set earlier via {@link #setOutputDir} otherwise {@code $buildDir/docs/doxygen}   .
     */
    @OutputDirectory
    File getOutputDir() {
        project.file(this.outputDir)
    }

    /** Sets the output directory
     *
     * @param dir A path that can be converetd with {@code project.file}.
     */
    void setOutputDir(Object dir) {
        this.outputDir = dir
    }

    /** Constructs a Doxygen task object and sets some default Doxygen properties
     *
     */
    Doxygen() {
        doxyUpdate.setProperty('PROJECT_NAME', project.name)
        doxyUpdate.setProperty('QUIET', project.logger.isQuietEnabled())
        doxyUpdate.setProperty('WARNINGS', !project.logger.isQuietEnabled())
    }

    /** Returns a map containing all of the executable paths that have been set
     *
     * @return
     */
    @CompileDynamic
    def getExecutables() { paths.asImmutable() }

    /** Allows for setting paths to various executables. By default they would be searched for
     * in the search path.
     *
     * @param cfg A configuration closure
     *
     * <li>doxygen
     * <li>mscgen
     */
    void executables(Closure cfg) {
        configureExecutables(cfg)
    }

    @Optional
    @InputFile
    File getTemplate() {
        this.templateFile
    }

    /** Sets the template Doxyfile to be used. If not supplied a default one will be
     * generated to be used as a template.
     *
     * @param tmpl Template Doxyfile
     */
    void template(final File tmpl) {
        templateFile = tmpl
    }

    /** Sets the template Doxyfile to be used. If not supplied a default one will be
     * generated to be used as a template.
     *
     * @param tmpl Template Doxyfile
     */
    void template(final String tmpl) {
        template project.file(tmpl)
    }

    /** Allows for one more image paths to be set
     *
     * @param paths
     */
    void image_path(final Object... paths) {

        def workaround = imagePaths

        paths.each {
            File fName
            switch (it) {
                case File:
                    fName = (File)it
                    break
                default:
                    fName = new File(it.toString())
            }

            if (fName.isDirectory()) {
                inputs.dir fName
            } else {
                inputs.file fName
            }

            workaround.add(fName)
        }
    }

    /** Alias for outputDir
     *
     * @param outdir
     */
    void output_directory(def outdir) {
        outputDir = outdir
    }


    @TaskAction
    void exec() {
        setDefaults()
        File doxyfile = createDoxyfile()
        project.logger.debug "Using ${doxyfile} as Doxygen configuration file"
        editDoxyfile(doxyfile)
        runDoxygen(doxyfile)
    }

    @CompileDynamic
    def methodMissing(String name, args) {

        switch (name) {
            case 'quiet':
            case 'warnings':
            case 'subgrouping':
            case 'recursive':
                doxyUpdate.setProperty(name, args[0])
                break

            case 'input':
                throw new DoxygenException("'${name}' is ignored, use 'source' and 'sourceDir' instead (with exclude patterns as appropriate).")
                break

            case 'mscgen_path':
            case 'dot_path':
            case 'perl_path':
            case 'hhc_location':
                throw new DoxygenException("'${name}' is ignored, use 'executables' closure instead.")
                break

            default:

                if (name.find(/.+_.+/) && name.matches(/[_\p{Digit}\p{Lower}]{3,}/)) {
                    if (args.size() == 1) {
                        doxyUpdate.setProperty(name, args[0])
                    } else {
                        doxyUpdate.setProperty(name, args)
                    }
                } else {
                    throw new DoxygenException("'${name}' is not a valid configuration option")
                }
        }

        true
    }

    /** Returns the current hashmap of Doxygen properties that will override settings in the Doxygen file
     */
    Map<String,String> getDoxygenProperties() {
        doxyUpdate.properties.asImmutable()
    }

    /** Set some default values in the doxyUpdate
     *
     */
    @groovy.transform.PackageScope
    @CompileDynamic
    void setDefaults() {

        if (imagePaths.size()) {
            doxyUpdate.setProperty('IMAGE_PATH', imagePaths as File[])
        }

        if(source.files.empty) {
            source 'src/main/cpp',
                'src/main/headers',
                'src/main/asm',
                'src/main/objectiveC',
                'src/main/objectiveCpp',
                'src/main/c'
        }
        
        doxyUpdate.setProperty('INPUT', source)
        doxyUpdate.setProperty('OUTPUT_DIRECTORY', getOutputDir() )

        if(doxyUpdate.properties['PROJECT_NUMBER'] == null) {
            doxyUpdate.setProperty('PROJECT_NUMBER', project.version)
        }

        def workaround = doxyUpdate
        paths.each { name,path ->
            if( name != 'doxygen' ) {
                workaround.setProperty(Executables.EXECUTABLES[name],project.file(path))
            }
        }
    }

    /** Creates a Doxyfile that will eventually be passed to the Doxygen executable.
     * If a template has been set, if will make a copy of that, otherwise it will call
     * Doxygen to generate a default file.
     *
     * @return The Doxyfile File instance.
     */
    private File createDoxyfile() {
        File doxyfile = new File(project.buildDir, "tmp/${project.name}.doxyfile")
        if(!doxyfile.parentFile.exists()) {
            doxyfile.parentFile.mkdirs()
        }
        if (templateFile) {
            if (!templateFile.exists()) {
                throw new DoxygenException("${templateFile} does not exist")
            }
            doxyfile.text = templateFile.text
        } else {
            runDoxygen(doxyfile, ['-g'])
        }

        return doxyfile
    }

    /** Edits a Doxyfile and replaces existing properties with ones passed down via
     * Gradle configuration.
     *
     * @param doxyfile
     */
    private void editDoxyfile(File doxyfile) {
        DoxyfileEditor editor = new DoxyfileEditor(logger: project.logger)
        editor.update(doxygenProperties, doxyfile)
    }

    /** Runs the Doxygen executable
     *
     * @param doxyfile
     * @param cmdargs
     */
    @CompileDynamic
    private void runDoxygen(final File doxyfile, def cmdargs = []) {

        cmdargs.add(doxyfile.absolutePath)
        ExecResult execResult = project.exec {
            executable  executables.doxygen.call()
            args        cmdargs
        }
    }

    @CompileDynamic
    private configureExecutables(Closure cfg) {
        Closure cl = cfg.clone()
        cl.delegate = new Executables(paths,project)
        cl.resolveStrategy = Closure.DELEGATE_FIRST
        cl.call()
    }

    private Map<String,Object> paths = [ doxygen: (Object){'doxygen'} ]
    private File templateFile = null
    private DoxygenProperties doxyUpdate = new DoxygenProperties()
    private List<File> imagePaths = []
    private Object outputDir = new File(project.buildDir, 'docs/doxygen')


}



