import React, { useState } from "react";
import axios from "axios";
import CodeEditor from "./components/CodeEditor";
import ErrorList from "./components/ErrorList";
import "./App.css";

function App() {
  const [code, setCode] = useState("var x = 5;\nprint(z);");
  const [errors, setErrors] = useState([]);
  const [loading, setLoading] = useState(false);

  const analyzeCode = async () => {
    setLoading(true);
    setErrors([]);
    try {
      const res = await axios.post("http://localhost:5000/analyze", { code });
      setErrors(res.data);
    } catch (err) {
      setErrors([{ line: 0, type: "Server", message: err.message }]);
    }
    setLoading(false);
  };

  return (
    <div className="App">
      <h1>🧩 Line-by-Line Error Reporter</h1>
      <p className="subtitle">A simple compiler front-end with live error analysis</p>
      <CodeEditor code={code} setCode={setCode} />
      <button className="analyze-btn" onClick={analyzeCode} disabled={loading}>
        {loading ? "Analyzing..." : "Analyze Code"}
      </button>
      <ErrorList errors={errors} />
    </div>
  );
}

export default App;
