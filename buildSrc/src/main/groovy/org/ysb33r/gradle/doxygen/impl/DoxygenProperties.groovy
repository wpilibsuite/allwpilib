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

import groovy.transform.CompileStatic
import org.gradle.api.file.FileCollection

/**
 * Created by schalkc on 21/05/2014.
 */
@CompileStatic
class DoxygenProperties {

    void setProperty(final String name, Boolean value) {
        doxyUpdate[doxName(name)] = value ? 'YES' : 'NO'
    }

     void setProperty(final String name, File value) {
        setProperty(name,value.absolutePath)
    }

     void setProperty(final String name, File[] values) {
        setProperty(name,values.collect { File it -> it.absolutePath } as String[])
    }

     void setProperty(final String name, FileCollection values) {
        setProperty(name,values.files.collect { it.absolutePath } as String[])
    }

     void setProperty(final String name, String value) {
        if(value =~ /\s+/ ) {
            doxyUpdate[doxName(name)] = "\"${value}\"".toString()
        } else {
            doxyUpdate[doxName(name)] = value
        }
    }

    void setProperty(final String name, String[] values) {
        def escaped = values.collect { value ->
            (value =~ /\s+/ ) ? "\"${value}\"" : value
        }
        doxyUpdate[doxName(name)] = escaped.join(' ')
    }

    void setProperty(final String name, Number value) {
        doxyUpdate[doxName(name)] = "${value}".toString()
    }

    void setProperty(final String name,Object[] values) {
        setProperty( name, values.collect { value ->
            switch(value) {
                case File:
                    return ((File)value).absolutePath

                default:
                    return value.toString()
            }
        } as String[] )
    }

    Map<String,String> getProperties() { this.doxyUpdate }

    private String doxName(final String name) {
        name.toUpperCase()
    }

    private final Map<String,String> doxyUpdate = [:]

}
