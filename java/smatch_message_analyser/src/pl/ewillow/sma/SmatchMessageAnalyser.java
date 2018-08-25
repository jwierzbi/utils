package pl.ewillow.sma;

import java.util.ArrayList;
import java.util.Collections;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class SmatchMessageAnalyser {
    private String activeFile;
    private final Stats stats = new Stats();
    private final PatternHandler[] patternHandlers;
    private final Map<String, Issues> errorsAndWarnings = new LinkedHashMap<>();

    public class Stats {
        private int warningCount;
        private int errorCount;

        public int getFileCount() { return errorsAndWarnings.size(); }
        public int getWarningCount() { return warningCount; }
        public int getErrorCount() { return errorCount; }

        @Override
        public String toString() {
            return String.format("%d error(s) and %d warning(s) in %d file(s)",
                    getErrorCount(), getWarningCount(), getFileCount());
        }
    }

    public static class SparseIssue {
        private final String filePath;
        private final int lineNumber;
        private final String message;

        public SparseIssue(String filePath, int lineNumber, String message) {
            this.filePath = filePath;
            this.lineNumber = lineNumber;
            this.message = message;
        }

        public String getFilePath() {
            return filePath;
        }

        public int getLineNumber() {
            return lineNumber;
        }

        public String getMessage() {
            return message;
        }
    }

    public static class SmatchIssue {
        private final String filePath;
        private final int lineNumber;
        private final String functionName;
        private final String message;

        public SmatchIssue(String filePath, int lineNumber, String functionName,
                String message) {
            this.filePath = filePath;
            this.lineNumber = lineNumber;
            this.functionName = functionName;
            this.message = message;
        }

        public String getFilePath() {
            return filePath;
        }

        public int getLineNumber() {
            return lineNumber;
        }

        public String getFunctionName() {
            return functionName;
        }

        public String getMessage() {
            return message;
        }
    }

    public static class Issues {
        private List<SparseIssue> sparseWarningList = new ArrayList<>();
        private List<SparseIssue> sparseErrorList = new ArrayList<>();
        private List<SmatchIssue> smatchWarningList = new ArrayList<>();
        private List<SmatchIssue> smatchErrorList = new ArrayList<>();
        private int issueCount = 0;

        private void addSparseWarning(SparseIssue warning) {
            sparseWarningList.add(warning);
            issueCount++;
        }

        private void addSparseError(SparseIssue warning) {
            sparseErrorList.add(warning);
            issueCount++;
        }

        private void addSmatchWarning(SmatchIssue warning) {
            smatchWarningList.add(warning);
            issueCount++;
        }

        private void addSmatchError(SmatchIssue warning) {
            smatchErrorList.add(warning);
            issueCount++;
        }

        public boolean hasIssues() {
            return issueCount > 0;
        }

        public List<SparseIssue> getSparseWarningList() {
            return Collections.unmodifiableList(sparseWarningList);
        }

        public List<SparseIssue> getSparseErrorList() {
            return Collections.unmodifiableList(sparseErrorList);
        }

        public List<SmatchIssue> getSmatchWarningList() {
            return Collections.unmodifiableList(smatchWarningList);
        }

        public List<SmatchIssue> getSmatchErrorList() {
            return Collections.unmodifiableList(smatchErrorList);
        }
    }

    private abstract class PatternHandler {
        private final Pattern pattern;

        public PatternHandler(String pattern) {
            this.pattern = Pattern.compile(pattern);
        }

        public abstract void handle(Matcher matcher);
    }

    public SmatchMessageAnalyser() {
        patternHandlers = new PatternHandler[] {
            /* TODO: this pattern should be configurable */
            new PatternHandler(
                "^\\s*CHECK\\s+(.+)$"
            ) {
                @Override
                public void handle(Matcher matcher) {
                    addNewFileIfNotPresent(matcher.group(1));
                    activeFile = getCanonicalPath(matcher.group(1));
                }
            },
            new PatternHandler(
                "^\\s*(.+):(\\d+):(\\d+):\\s+warning:\\s+(.+)$"
            ) {
                @Override
                public void handle(Matcher matcher) {
                    String filePath = activeFile;

                    /* FIXME: if activeFile is not null and makefile output
                     *        synchronisation is not enabled this can
                     *        be attributed to a wrong file */
                    if (activeFile == null) {
                        filePath = getCanonicalPath(matcher.group(1));
                        addNewFileIfNotPresent(filePath);
                    }

                    errorsAndWarnings.get(filePath)
                            .addSparseWarning(new SparseIssue(
                                getCanonicalPath(matcher.group(1)),
                                Integer.parseInt(matcher.group(2)),
                                matcher.group(4)
                            ));

                    stats.warningCount++;
                }
            },
            new PatternHandler(
                "^\\s*(.+):(\\d+):(\\d+):\\s+error:\\s+(.+)$"
            ) {
                @Override
                public void handle(Matcher matcher) {
                    String filePath = activeFile;

                    /* FIXME: if activeFile is not null and makefile output
                     *        synchronisation is not enabled this can
                     *        be attributed to a wrong file */
                    if (activeFile == null) {
                        filePath = getCanonicalPath(matcher.group(1));
                        addNewFileIfNotPresent(filePath);
                    }

                    errorsAndWarnings.get(filePath)
                            .addSparseError(new SparseIssue(
                                getCanonicalPath(matcher.group(1)),
                                Integer.parseInt(matcher.group(2)),
                                matcher.group(4)
                            ));

                    stats.errorCount++;
                }
            },
            new PatternHandler(
                "^\\s*(.+):(\\d+)\\s+(\\w+)\\(\\)\\s+warn:\\s+(.+)$"
            ) {
                @Override
                public void handle(Matcher matcher) {
                    String filePath = getCanonicalPath(matcher.group(1));
                    addNewFileIfNotPresent(filePath);

                    errorsAndWarnings.get(filePath)
                            .addSmatchWarning(new SmatchIssue(
                                filePath,
                                Integer.parseInt(matcher.group(2)),
                                matcher.group(3),
                                matcher.group(4)
                            ));

                    stats.warningCount++;
                }
            },
            new PatternHandler(
                "^\\s*(.+):(\\d+)\\s+(\\w+)\\(\\)\\s+error:\\s+(.+)$"
            ) {
                @Override
                public void handle(Matcher matcher) {
                    String filePath = getCanonicalPath(matcher.group(1));
                    addNewFileIfNotPresent(filePath);

                    errorsAndWarnings.get(filePath)
                            .addSmatchError(new SmatchIssue(
                                filePath,
                                Integer.parseInt(matcher.group(2)),
                                matcher.group(3),
                                matcher.group(4)
                            ));

                    stats.errorCount++;
                }
            }
        };
    }

    private void addNewFileIfNotPresent(String filePath) {
        if (!errorsAndWarnings.containsKey(filePath))
            errorsAndWarnings.put(filePath, new Issues());
    }

    private String getCanonicalPath(String filePath) {
        try {
            return new java.io.File(filePath).getCanonicalPath();
        } catch (java.io.IOException ex) {
            return filePath;
        }
    }

    public void process(List<String> log) {
        // clean eaverything in case we're run multiple times
        clearState();

        for (String line : log) {
            for (PatternHandler ph : patternHandlers) {
                Matcher m = ph.pattern.matcher(line);
                if (m.matches()) {
                    ph.handle(m);
                    break;
                }
            }
        }

        errorsAndWarnings.entrySet().removeIf(el -> !el.getValue().hasIssues());
    }

    private void clearState() {
        stats.warningCount = 0;
        stats.errorCount = 0;
        activeFile = null;
        errorsAndWarnings.clear();
    }

    public Stats getStats() {
        return stats;
    }

    public Map<String, Issues> getErrorsAndWarnings() {
        return errorsAndWarnings;
    }
}
