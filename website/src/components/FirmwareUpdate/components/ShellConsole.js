import React, { useState, useRef, useEffect, useCallback, useMemo } from "react";

const COMMAND_TIMEOUT_MS = 5000;

const sanitizeLines = (input) => {
  if (!input) {
    return [];
  }

  const source = Array.isArray(input) ? input : [input];

  return source
    .filter(item => typeof item === "string")
    .flatMap(item => item.replace(/\r/g, "").split("\n"))
    .map(line => line.replace(/\s+$/g, ""));
};

const ShellConsole = ({
  mcumgr,
  registerShellListener,
  isConnected,
  className = "",
  textareaClassName = "",
}) => {
  const [logLines, setLogLines] = useState(["Connect first", "Type 'help' to list commands."]);
  const [currentInput, setCurrentInput] = useState("");

  const textareaRef = useRef(null);
  const pendingIdsRef = useRef([]);
  const pendingMetaRef = useRef(new Map());
  const timeoutMapRef = useRef(new Map());
  const idCounterRef = useRef(0);
  const tabNoticeShownRef = useRef(false);

  const appendLines = useCallback((lines) => {
    const sanitized = sanitizeLines(lines);
    if (sanitized.length === 0) {
      return;
    }

    setLogLines(prev => [...prev, ...sanitized]);
  }, []);

  const consoleValue = useMemo(() => {
    const history = logLines.length ? `${logLines.join("\n")}\n` : "";
    return `${history}$ ${currentInput}`;
  }, [logLines, currentInput]);

  const clearPending = useCallback((id) => {
    pendingIdsRef.current = pendingIdsRef.current.filter(value => value !== id);
    pendingMetaRef.current.delete(id);
  }, []);

  const handleSendFailure = useCallback((id, error) => {
    const timer = timeoutMapRef.current.get(id);
    if (timer) {
      clearTimeout(timer);
    }
    timeoutMapRef.current.delete(id);

    const meta = pendingMetaRef.current.get(id);
    clearPending(id);

    const message = error?.message || "Failed to send command";

    appendLines(meta?.suppressOutput ? message : `  !! ${message}`);
  }, [appendLines, clearPending]);

  const sendShellCommand = useCallback((commandString, { suppressOutput = false } = {}) => {
    if (!mcumgr || typeof mcumgr.cmdShellExec !== "function") {
      return;
    }

    const trimmed = commandString.trim();
    if (trimmed.length === 0) {
      return;
    }

    const id = idCounterRef.current += 1;

    if (!suppressOutput) {
      appendLines(`$ ${trimmed}`);
    }

    pendingIdsRef.current.push(id);
    pendingMetaRef.current.set(id, {
      suppressOutput,
      command: trimmed,
    });

    const timer = setTimeout(() => {
      timeoutMapRef.current.delete(id);
      const meta = pendingMetaRef.current.get(id);
      clearPending(id);

      if (meta?.suppressOutput) {
        appendLines("Shell request timed out");
      } else {
        appendLines("  !! timeout");
      }
    }, COMMAND_TIMEOUT_MS);

    timeoutMapRef.current.set(id, timer);

    try {
      const result = mcumgr.cmdShellExec([commandString]);
      if (result && typeof result.then === "function") {
        result.catch(error => handleSendFailure(id, error));
      }
    } catch (error) {
      handleSendFailure(id, error);
    }
  }, [appendLines, clearPending, handleSendFailure, mcumgr]);

  const handleShellResponse = useCallback((message) => {
    const { data = {} } = message || {};

    if (pendingIdsRef.current.length === 0) {
      return;
    }

    const currentId = pendingIdsRef.current.shift();

    const timer = timeoutMapRef.current.get(currentId);
    if (timer) {
      clearTimeout(timer);
    }
    timeoutMapRef.current.delete(currentId);

    const meta = pendingMetaRef.current.get(currentId);
    clearPending(currentId);

    const shellOutput = typeof data.o === "string" ? data.o : "";
    const shellReturn = typeof data.ret === "number" ? data.ret : null;

    const outputLines = sanitizeLines(shellOutput);
    if (outputLines.length > 0) {
      appendLines(outputLines);
    }

    if (typeof shellReturn === "number") {
      const prefix = shellReturn === 0 ? "  ret" : "  !! ret";
      appendLines(`${prefix} ${shellReturn}`);
    }

    if (data && typeof data === "object" && data.err) {
      const { group, rc } = data.err;
      const parts = ["  mgmt rc", rc];
      if (group !== undefined) {
        parts.push("group", group);
      }
      appendLines(parts.join(" "));
    }
  }, [appendLines, clearPending]);

  useEffect(() => {
    if (!registerShellListener) {
      return undefined;
    }

    const unsubscribe = registerShellListener(handleShellResponse);
    return () => {
      if (unsubscribe) {
        unsubscribe();
      }
    };
  }, [registerShellListener, handleShellResponse]);

  useEffect(() => () => {
    timeoutMapRef.current.forEach(timer => clearTimeout(timer));
    timeoutMapRef.current.clear();
    pendingIdsRef.current = [];
    pendingMetaRef.current.clear();
  }, []);

  useEffect(() => {
    const textarea = textareaRef.current;
    if (!textarea) {
      return;
    }

    textarea.selectionStart = consoleValue.length;
    textarea.selectionEnd = consoleValue.length;
    textarea.scrollTop = textarea.scrollHeight;
  }, [consoleValue]);

  useEffect(() => {
    if (!isConnected) {
      return;
    }

    const textarea = textareaRef.current;
    if (textarea) {
      textarea.focus();
    }
  }, [isConnected]);

  const handleSubmit = useCallback(() => {
    if (!isConnected) {
      return;
    }

    const toSend = currentInput;
    setCurrentInput("");
    sendShellCommand(toSend, { logCommand: true });
  }, [currentInput, isConnected, sendShellCommand]);

  const handleTab = useCallback(() => {
    if (!isConnected) {
      return;
    }

    if (!tabNoticeShownRef.current) {
      appendLines("Tab completion is not supported over the mcumgr shell interface.");
      tabNoticeShownRef.current = true;
    }
  }, [appendLines, isConnected]);
 
  const handleTextareaChange = useCallback((event) => {
    if (!isConnected) {
      return;
    }

    const newValue = event.target.value;
    const historyPrefix = logLines.length ? `${logLines.join("\n")}\n` : "";
    const promptPrefixLength = historyPrefix.length + 2; // account for "$ "

    if (!newValue.startsWith(historyPrefix) || newValue.length < promptPrefixLength) {
      event.target.value = consoleValue;
      return;
    }

    setCurrentInput(newValue.slice(promptPrefixLength));
  }, [consoleValue, isConnected, logLines]);

  const handleTextareaKeyDown = useCallback((event) => {
    if (!isConnected) {
      event.preventDefault();
      return;
    }

    if (event.key === "Enter") {
      event.preventDefault();
      handleSubmit();
      return;
    }

    if (event.key === "Tab") {
      event.preventDefault();
      handleTab();
      return;
    }

    if (event.key === "Backspace" && currentInput.length === 0) {
      event.preventDefault();
    }
  }, [currentInput.length, handleSubmit, handleTab, isConnected]);

  const containerClasses = [
    "bg-white dark:bg-white/5 backdrop-blur-sm rounded-lg p-3 shadow-sm",
    className,
  ]
    .filter(Boolean)
    .join(" ");

  const textareaClasses = [
    "w-full h-64 bg-black text-green-200 font-mono text-xs rounded-md p-3 resize-none focus:outline-none focus:ring-2 focus:ring-zswatch-primary/40",
    textareaClassName,
  ]
    .filter(Boolean)
    .join(" ");

  return (
    <div className={containerClasses}>
      <textarea
        ref={textareaRef}
        value={consoleValue}
        onChange={handleTextareaChange}
        onKeyDown={handleTextareaKeyDown}
        spellCheck={false}
        readOnly={!isConnected}
        className={textareaClasses}
      />
    </div>
  );
};

export default ShellConsole;
