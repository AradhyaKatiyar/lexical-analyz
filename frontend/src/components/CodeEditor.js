import React from "react";
import CodeMirror from "@uiw/react-codemirror";
import { javascript } from "@codemirror/lang-javascript";

export default function CodeEditor({ code, setCode }) {
  return (
    <div className="editor-container">
      <CodeMirror
        value={code}
        height="300px"
        theme="dark"
        extensions={[javascript()]}
        onChange={(value) => setCode(value)}
      />
    </div>
  );
}
