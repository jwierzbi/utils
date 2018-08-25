# SmatchMessageAnalyser

Simple class for analysing Smatch build output log.

For details about Smatch visit: http://smatch.sourceforge.net/.

## Build

### Requirements

- JDK 1.8
- Gradle >= 4.10

### Build

To build simple issue a gradle command:

    gradle build

## Usage

Below example reads a log from a file and displays all of the warnings and
errors found in the log:

```java
import java.io.File;
import java.nio.charset.Charset;
import java.nio.file.Files;
import java.util.List;
import java.util.Map;
import pl.ewillow.sma.SmatchMessageAnalyser;
import static pl.ewillow.sma.SmatchMessageAnalyser.Issues;

public class Main {
    public static void main(String args[]) throws Exception {
        // load file to list of strings
        List<String> log = java.nio.file.Files.readAllLines(
            new File("../smatch_warnings.tmp").toPath(),
            Charset.defaultCharset()
        );

        // create the instance of the analyser and run it
        SmatchMessageAnalyser a = new SmatchMessageAnalyser();
        a.process(log);

        // show the stats
        System.out.println(a.getStats());

        // and all of the issues
        a.getErrorsAndWarnings().forEach((file, issues) -> {
            System.out.println("file: " + file);
            issues.getSparseWarningList().forEach((issue) -> {
                System.out.println(String.format("WARN: %s:%d: %s",
                        issue.getFilePath(), issue.getLineNumber(),
                        issue.getMessage()));
            });
            issues.getSmatchWarningList().forEach((issue) -> {
                System.out.println(String.format("WARN: %s:%d: (%s()) %s",
                        issue.getFilePath(), issue.getLineNumber(),
                        issue.getFunctionName(), issue.getMessage()));
            });
            issues.getSparseErrorList().forEach((issue) -> {
                System.out.println(String.format("ERROR: %s:%d: %s",
                        issue.getFilePath(), issue.getLineNumber(),
                        issue.getMessage()));
            });
            issues.getSmatchErrorList().forEach((issue) -> {
                System.out.println(String.format("ERROR: %s:%d: (%s()) %s",
                        issue.getFilePath(), issue.getLineNumber(),
                        issue.getFunctionName(), issue.getMessage()));
            });
        });
    }
}
```
