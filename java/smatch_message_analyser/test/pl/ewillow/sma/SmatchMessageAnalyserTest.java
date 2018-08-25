package pl.ewillow.sma;

import java.util.Arrays;
import java.util.Iterator;
import java.util.Map;
import org.junit.jupiter.api.Test;
import static pl.ewillow.sma.SmatchMessageAnalyser.Issues;
import static org.junit.jupiter.api.Assertions.*;

public class SmatchMessageAnalyserTest {
    @Test
    void processEmptyBuildLog() {
        SmatchMessageAnalyser a = new SmatchMessageAnalyser();

        a.process(Arrays.asList(""));

        assertTrue(a.getErrorsAndWarnings().isEmpty());
    }

    @Test
    void processLogWithoutErrorsAndWarnings() {
        SmatchMessageAnalyser a = new SmatchMessageAnalyser();

        a.process(Arrays.asList(
            "CC [M]  /build/kbuild/subdir/subdir/linux/file1.o",
            "CHECK   /build/kbuild/subdir/subdir/linux/file1.c",
            "CC [M]  /build/kbuild/subdir/subdir/linux/file2.o",
            "CHECK   /build/kbuild/subdir/subdir/linux/file2.c",
            "CC [M]  /build/kbuild/subdir/subdir/linux/file3.o",
            "CHECK   /build/kbuild/subdir/subdir/linux/file3.c",
            "CC [M]  /build/kbuild/subdir/subdir/linux/file4.o",
            "CHECK   /build/kbuild/subdir/subdir/linux/file4.c"
        ));

        assertTrue(a.getErrorsAndWarnings().isEmpty());
    }

    @Test
    void processLogWithSparseWarning() {
        SmatchMessageAnalyser a = new SmatchMessageAnalyser();

        a.process(Arrays.asList(
            "CHECK   /build/kbuild/subdir/subdir/generated/file0.c",
            "/opt/gcc_aarch64-linux-gnu/bin/../lib/gcc/aarch64-linux-gnu/4.9.3/include/stddef.h:413:9: warning: preprocessor token redefined",
            "include/linux/stddef.h:17:9: this was the original definition",
            "CC [M]  /build/kbuild/subdir/subdir/generated/generated/file0.o",
            "CHECK   /build/kbuild/subdir/subdir/linux/file2.c",
            "CC [M]  /build/kbuild/subdir/subdir/linux/file2.o",
            "CHECK   /build/kbuild/subdir/subdir/linux/file3.c",
            "CC [M]  /build/kbuild/subdir/subdir/linux/file3.o"
        ));

        Map<String, Issues> data = a.getErrorsAndWarnings();

        Map.Entry entry = data.entrySet().iterator().next();
        Issues issues = (Issues) entry.getValue();
        assertEquals(1, data.size());
        assertEquals("/build/kbuild/subdir/subdir/generated/file0.c",
                entry.getKey());
        assertEquals("/opt/gcc_aarch64-linux-gnu/lib/gcc/aarch64-linux-gnu/4.9.3/include/stddef.h",
                (issues.getSparseWarningList().get(0).getFilePath()));
        assertEquals(413,
                (issues.getSparseWarningList().get(0).getLineNumber()));
        assertEquals("preprocessor token redefined",
                (issues.getSparseWarningList().get(0).getMessage()));
    }

    @Test
    void processLogWithSparseWarningWithoutCheckLine() {
        SmatchMessageAnalyser a = new SmatchMessageAnalyser();

        a.process(Arrays.asList(
            "/opt/gcc_aarch64-linux-gnu/bin/../lib/gcc/aarch64-linux-gnu/4.9.3/include/stddef.h:413:9: warning: preprocessor token redefined",
            "include/linux/stddef.h:17:9: this was the original definition",
            "CC [M]  /build/kbuild/subdir/subdir/generated/generated/file0.o",
            "CC [M]  /build/kbuild/subdir/subdir/linux/file2.o",
            "CC [M]  /build/kbuild/subdir/subdir/linux/file3.o"
        ));

        Map<String, Issues> data = a.getErrorsAndWarnings();

        Map.Entry entry = data.entrySet().iterator().next();
        Issues issues = (Issues) entry.getValue();
        assertEquals(1, data.size());
        assertEquals("/opt/gcc_aarch64-linux-gnu/lib/gcc/aarch64-linux-gnu/4.9.3/include/stddef.h",
                entry.getKey());
        assertEquals("/opt/gcc_aarch64-linux-gnu/lib/gcc/aarch64-linux-gnu/4.9.3/include/stddef.h",
                (issues.getSparseWarningList().get(0).getFilePath()));
        assertEquals(413,
                (issues.getSparseWarningList().get(0).getLineNumber()));
        assertEquals("preprocessor token redefined",
                (issues.getSparseWarningList().get(0).getMessage()));
    }

    @Test
    void processLogWithSparseError() {
        SmatchMessageAnalyser a = new SmatchMessageAnalyser();

        a.process(Arrays.asList(
            "CHECK   /build/kbuild/subdir/subdir/generated/file0.c",
            "CC [M]  /build/kbuild/subdir/subdir/generated/generated/file0.o",
            "CHECK   /build/kbuild/subdir/subdir/linux/file2.c",
            "CC [M]  /build/kbuild/subdir/subdir/linux/file2.o",
            "CHECK   /build/kbuild/subdir/subdir/linux/file3.c",
            "/opt/gcc_aarch64-linux-gnu/bin/../lib/gcc/aarch64-linux-gnu/4.9.3/include/stddef.h:413:9: error: preprocessor token redefined",
            "CC [M]  /build/kbuild/subdir/subdir/linux/file3.o"
        ));

        Map<String, Issues> data = a.getErrorsAndWarnings();

        Map.Entry entry = data.entrySet().iterator().next();
        Issues issues = (Issues) entry.getValue();
        assertEquals(1, data.size());
        assertEquals("/build/kbuild/subdir/subdir/linux/file3.c",
                entry.getKey());
        assertEquals("/opt/gcc_aarch64-linux-gnu/lib/gcc/aarch64-linux-gnu/4.9.3/include/stddef.h",
                (issues.getSparseErrorList().get(0).getFilePath()));
        assertEquals(413,
                (issues.getSparseErrorList().get(0).getLineNumber()));
        assertEquals("preprocessor token redefined",
                (issues.getSparseErrorList().get(0).getMessage()));
    }

    @Test
    void processLogWithSparseErrorWithoutCheckLine() {
        SmatchMessageAnalyser a = new SmatchMessageAnalyser();

        a.process(Arrays.asList(
            "CC [M]  /build/kbuild/subdir/subdir/generated/generated/file0.o",
            "CC [M]  /build/kbuild/subdir/subdir/linux/file2.o",
            "/opt/gcc_aarch64-linux-gnu/bin/../lib/gcc/aarch64-linux-gnu/4.9.3/include/stddef.h:413:9: error: preprocessor token redefined",
            "CC [M]  /build/kbuild/subdir/subdir/linux/file3.o"
        ));

        Map<String, Issues> data = a.getErrorsAndWarnings();

        Map.Entry entry = data.entrySet().iterator().next();
        Issues issues = (Issues) entry.getValue();
        assertEquals(1, data.size());
        assertEquals("/opt/gcc_aarch64-linux-gnu/lib/gcc/aarch64-linux-gnu/4.9.3/include/stddef.h",
                entry.getKey());
        assertEquals("/opt/gcc_aarch64-linux-gnu/lib/gcc/aarch64-linux-gnu/4.9.3/include/stddef.h",
                (issues.getSparseErrorList().get(0).getFilePath()));
        assertEquals(413,
                (issues.getSparseErrorList().get(0).getLineNumber()));
        assertEquals("preprocessor token redefined",
                (issues.getSparseErrorList().get(0).getMessage()));
    }

    @Test
    void processLogWithSmatchWarning() {
        SmatchMessageAnalyser a = new SmatchMessageAnalyser();

        a.process(Arrays.asList(
            "CHECK   /build/kbuild/subdir/subdir/linux/file1.c",
            "CC [M]  /build/kbuild/subdir/subdir/linux/file1.o",
            "CHECK   /build/kbuild/subdir/subdir/linux/file2.c",
            "/build/kbuild/subdir/subdir/linux/file2.c:100 foo() warn: unused return: bar = (null)()",
            "CC [M]  /build/kbuild/subdir/subdir/linux/file2.o",
            "CHECK   /build/kbuild/subdir/subdir/linux/file3.c",
            "CC [M]  /build/kbuild/subdir/subdir/linux/file3.o"
        ));

        Map<String, Issues> data = a.getErrorsAndWarnings();

        assertEquals(1, data.size());
        assertEquals("/build/kbuild/subdir/subdir/linux/file2.c",
                data.keySet().iterator().next());


        Map.Entry entry = data.entrySet().iterator().next();
        Issues issues = (Issues) entry.getValue();
        assertEquals(1, data.size());
        assertEquals("/build/kbuild/subdir/subdir/linux/file2.c",
                entry.getKey());
        assertEquals("/build/kbuild/subdir/subdir/linux/file2.c",
                (issues.getSmatchWarningList().get(0).getFilePath()));
        assertEquals(100,
                (issues.getSmatchWarningList().get(0).getLineNumber()));
        assertEquals("foo",
                (issues.getSmatchWarningList().get(0).getFunctionName()));
        assertEquals("unused return: bar = (null)()",
                (issues.getSmatchWarningList().get(0).getMessage()));
    }

    @Test
    void processLogWithSmatchWarningWithoutCheckLine() {
        SmatchMessageAnalyser a = new SmatchMessageAnalyser();

        a.process(Arrays.asList(
            "CC [M]  /build/kbuild/subdir/subdir/linux/file1.o",
            "/build/kbuild/subdir/subdir/linux/file2.c:100 foo() warn: unused return: bar = (null)()",
            "CC [M]  /build/kbuild/subdir/subdir/linux/file2.o",
            "CC [M]  /build/kbuild/subdir/subdir/linux/file3.o"
        ));

        Map<String, Issues> data = a.getErrorsAndWarnings();

        assertEquals(1, data.size());
        assertEquals("/build/kbuild/subdir/subdir/linux/file2.c",
                data.keySet().iterator().next());


        Map.Entry entry = data.entrySet().iterator().next();
        Issues issues = (Issues) entry.getValue();
        assertEquals(1, data.size());
        assertEquals("/build/kbuild/subdir/subdir/linux/file2.c",
                entry.getKey());
        assertEquals("/build/kbuild/subdir/subdir/linux/file2.c",
                (issues.getSmatchWarningList().get(0).getFilePath()));
        assertEquals(100,
                (issues.getSmatchWarningList().get(0).getLineNumber()));
        assertEquals("foo",
                (issues.getSmatchWarningList().get(0).getFunctionName()));
        assertEquals("unused return: bar = (null)()",
                (issues.getSmatchWarningList().get(0).getMessage()));
    }

    @Test
    void processLogWithSmatchError() {
        SmatchMessageAnalyser a = new SmatchMessageAnalyser();

        a.process(Arrays.asList(
            "CHECK   /build/kbuild/subdir/subdir/linux/file1.c",
            "CC [M]  /build/kbuild/subdir/subdir/linux/file1.o",
            "CHECK   /build/kbuild/subdir/subdir/linux/file2.c",
            "CC [M]  /build/kbuild/subdir/subdir/linux/file2.o",
            "CHECK   /build/kbuild/subdir/subdir/linux/file3.c",
            "/build/kbuild/subdir/subdir/linux/file3.c:1118 FooBar() error: we previously assumed 's->member' could be null (see line 1250)",
            "CC [M]  /build/kbuild/subdir/subdir/linux/file3.o"
        ));

        Map<String, Issues> data = a.getErrorsAndWarnings();

        Map.Entry entry = data.entrySet().iterator().next();
        Issues issues = (Issues) entry.getValue();
        assertEquals(1, data.size());
        assertEquals("/build/kbuild/subdir/subdir/linux/file3.c",
                entry.getKey());
        assertEquals("/build/kbuild/subdir/subdir/linux/file3.c",
                (issues.getSmatchErrorList().get(0).getFilePath()));
        assertEquals(1118,
                (issues.getSmatchErrorList().get(0).getLineNumber()));
        assertEquals("FooBar",
                (issues.getSmatchErrorList().get(0).getFunctionName()));
        assertEquals("we previously assumed 's->member' could be null (see line 1250)",
                (issues.getSmatchErrorList().get(0).getMessage()));
    }

    @Test
    void processLogWithSmatchErrorWithoutCheckLine() {
        SmatchMessageAnalyser a = new SmatchMessageAnalyser();

        a.process(Arrays.asList(
            "CC [M]  /build/kbuild/subdir/subdir/linux/file1.o",
            "CC [M]  /build/kbuild/subdir/subdir/linux/file2.o",
            "/build/kbuild/subdir/subdir/linux/file3.c:1118 FooBar() error: we previously assumed 's->member' could be null (see line 1250)",
            "CC [M]  /build/kbuild/subdir/subdir/linux/file3.o"
        ));

        Map<String, Issues> data = a.getErrorsAndWarnings();

        Map.Entry entry = data.entrySet().iterator().next();
        Issues issues = (Issues) entry.getValue();
        assertEquals(1, data.size());
        assertEquals("/build/kbuild/subdir/subdir/linux/file3.c",
                entry.getKey());
        assertEquals("/build/kbuild/subdir/subdir/linux/file3.c",
                (issues.getSmatchErrorList().get(0).getFilePath()));
        assertEquals(1118,
                (issues.getSmatchErrorList().get(0).getLineNumber()));
        assertEquals("FooBar",
                (issues.getSmatchErrorList().get(0).getFunctionName()));
        assertEquals("we previously assumed 's->member' could be null (see line 1250)",
                (issues.getSmatchErrorList().get(0).getMessage()));
    }

    @Test
    void processLogWithMultipleWarningsAndErrors() {
        SmatchMessageAnalyser a = new SmatchMessageAnalyser();

        a.process(Arrays.asList(
            "CHECK   /build/kbuild/subdir/subdir/generated/file0.c",
            "/opt/gcc_aarch64-linux-gnu/bin/../lib/gcc/aarch64-linux-gnu/4.9.3/include/stddef.h:413:9: error: preprocessor token T0 redefined",
            "/opt/gcc_aarch64-linux-gnu/bin/../lib/gcc/aarch64-linux-gnu/4.9.3/include/stddef.h:413:9: error: preprocessor token T1 redefined",
            "/opt/gcc_aarch64-linux-gnu/bin/../lib/gcc/aarch64-linux-gnu/4.9.3/include/stddef.h:413:9: error: preprocessor token T2 redefined",
            "/opt/gcc_aarch64-linux-gnu/bin/../lib/gcc/aarch64-linux-gnu/4.9.3/include/stddef.h:413:9: warning: preprocessor token T1 redefined",
            "/opt/gcc_aarch64-linux-gnu/bin/../lib/gcc/aarch64-linux-gnu/4.9.3/include/stddef.h:413:9: warning: preprocessor token T2 redefined",
            "/build/kbuild/subdir/subdir/generated/file0.c:300 foo3() warn: unused return: bar3 = (null)()",
            "CC [M]  /build/kbuild/subdir/subdir/generated/file0.o",
            "CHECK   /build/kbuild/subdir/subdir/linux/file2.c",
            "/opt/gcc_aarch64-linux-gnu/bin/../lib/gcc/aarch64-linux-gnu/4.9.3/include/stddef.h:413:9: error: preprocessor token redefined",
            "/build/kbuild/subdir/subdir/linux/file2.c:100 foo() warn: unused return: bar1 = (null)()",
            "/build/kbuild/subdir/subdir/linux/file2.c:120 foo2() warn: unused return: bar2 = (null)()",
            "/build/kbuild/subdir/subdir/linux/file2.c:1118 FooBar() error: we previously assumed 's->member' could be null (see line 1250)",
            "/build/kbuild/subdir/subdir/linux/file2.c:2218 FooBaz() error: we previously assumed 's->member' could be null (see line 1250)",
            "CC [M]  /build/kbuild/subdir/subdir/linux/file2.o"
        ));

        Map.Entry entry;
        Issues issues;
        Map<String, Issues> data = a.getErrorsAndWarnings();
        Iterator<Map.Entry<String, Issues>> it = data.entrySet().iterator();
        assertEquals(2, data.size());

        // stats

        SmatchMessageAnalyser.Stats stats = a.getStats();
        assertEquals(2, stats.getFileCount());
        assertEquals(5, stats.getWarningCount());
        assertEquals(6, stats.getErrorCount());

        // file 0

        entry = it.next();
        issues = (Issues) entry.getValue();
        assertEquals("/build/kbuild/subdir/subdir/generated/file0.c", entry.getKey());
        assertTrue(issues.getSmatchErrorList().isEmpty());

        assertEquals("/opt/gcc_aarch64-linux-gnu/lib/gcc/aarch64-linux-gnu/4.9.3/include/stddef.h", issues.getSparseWarningList().get(0).getFilePath());
        assertEquals(413, issues.getSparseWarningList().get(0).getLineNumber());
        assertEquals("preprocessor token T1 redefined", issues.getSparseWarningList().get(0).getMessage());
        assertEquals("/opt/gcc_aarch64-linux-gnu/lib/gcc/aarch64-linux-gnu/4.9.3/include/stddef.h", issues.getSparseWarningList().get(1).getFilePath());
        assertEquals(413, issues.getSparseWarningList().get(1).getLineNumber());
        assertEquals("preprocessor token T2 redefined", issues.getSparseWarningList().get(1).getMessage());

        assertEquals("/opt/gcc_aarch64-linux-gnu/lib/gcc/aarch64-linux-gnu/4.9.3/include/stddef.h", issues.getSparseErrorList().get(0).getFilePath());
        assertEquals(413, issues.getSparseErrorList().get(0).getLineNumber());
        assertEquals("preprocessor token T0 redefined", issues.getSparseErrorList().get(0).getMessage());
        assertEquals("/opt/gcc_aarch64-linux-gnu/lib/gcc/aarch64-linux-gnu/4.9.3/include/stddef.h", issues.getSparseErrorList().get(1).getFilePath());
        assertEquals(413, issues.getSparseErrorList().get(1).getLineNumber());
        assertEquals("preprocessor token T1 redefined", issues.getSparseErrorList().get(1).getMessage());
        assertEquals("/opt/gcc_aarch64-linux-gnu/lib/gcc/aarch64-linux-gnu/4.9.3/include/stddef.h", issues.getSparseErrorList().get(2).getFilePath());
        assertEquals(413, issues.getSparseErrorList().get(2).getLineNumber());
        assertEquals("preprocessor token T2 redefined", issues.getSparseErrorList().get(2).getMessage());

        assertEquals("/build/kbuild/subdir/subdir/generated/file0.c", issues.getSmatchWarningList().get(0).getFilePath());
        assertEquals(300, issues.getSmatchWarningList().get(0).getLineNumber());
        assertEquals("foo3", issues.getSmatchWarningList().get(0).getFunctionName());
        assertEquals("unused return: bar3 = (null)()", issues.getSmatchWarningList().get(0).getMessage());

        // file 2

        entry = it.next();
        issues = (Issues) entry.getValue();
        assertEquals("/build/kbuild/subdir/subdir/linux/file2.c", entry.getKey());
        assertTrue(issues.getSparseWarningList().isEmpty());

        assertEquals("/opt/gcc_aarch64-linux-gnu/lib/gcc/aarch64-linux-gnu/4.9.3/include/stddef.h", issues.getSparseErrorList().get(0).getFilePath());
        assertEquals(413, issues.getSparseErrorList().get(0).getLineNumber());
        assertEquals("preprocessor token redefined", issues.getSparseErrorList().get(0).getMessage());

        assertEquals("/build/kbuild/subdir/subdir/linux/file2.c", issues.getSmatchWarningList().get(0).getFilePath());
        assertEquals(100, issues.getSmatchWarningList().get(0).getLineNumber());
        assertEquals("foo", issues.getSmatchWarningList().get(0).getFunctionName());
        assertEquals("unused return: bar1 = (null)()", issues.getSmatchWarningList().get(0).getMessage());
        assertEquals("/build/kbuild/subdir/subdir/linux/file2.c", issues.getSmatchWarningList().get(1).getFilePath());
        assertEquals(120, issues.getSmatchWarningList().get(1).getLineNumber());
        assertEquals("foo2", issues.getSmatchWarningList().get(1).getFunctionName());
        assertEquals("unused return: bar2 = (null)()", issues.getSmatchWarningList().get(1).getMessage());

        assertEquals("/build/kbuild/subdir/subdir/linux/file2.c", issues.getSmatchErrorList().get(0).getFilePath());
        assertEquals(1118, issues.getSmatchErrorList().get(0).getLineNumber());
        assertEquals("FooBar", issues.getSmatchErrorList().get(0).getFunctionName());
        assertEquals("we previously assumed 's->member' could be null (see line 1250)", issues.getSmatchErrorList().get(0).getMessage());
        assertEquals("/build/kbuild/subdir/subdir/linux/file2.c", issues.getSmatchErrorList().get(1).getFilePath());
        assertEquals(2218, issues.getSmatchErrorList().get(1).getLineNumber());
        assertEquals("FooBaz", issues.getSmatchErrorList().get(1).getFunctionName());
        assertEquals("we previously assumed 's->member' could be null (see line 1250)", issues.getSmatchErrorList().get(1).getMessage());
    }
}
