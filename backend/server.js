import express from "express";
import { exec } from "child_process";
import bodyParser from "body-parser";
import cors from "cors";
import path from "path";
import fs from "fs";

const app = express();
app.use(cors());
app.use(bodyParser.json());

app.post("/analyze", (req, res) => {
  const code = req.body.code;
  if (!code) return res.status(400).json({ error: "No code provided" });

  const exePath = path.join(process.cwd(), "error_reporter.exe");

  // 🧠 Check if file exists
  if (!fs.existsSync(exePath)) {
    console.error("❌ error_reporter.exe not found at:", exePath);
    return res.status(500).json({ error: "error_reporter.exe not found" });
  }

  const child = exec(`"${exePath}"`, (error, stdout, stderr) => {
    console.log("📤 STDOUT:", stdout);
    console.log("⚠️ STDERR:", stderr);

    if (error) {
      console.error("❌ Exec error:", error.message);
      return res
        .status(500)
        .json({ error: "Execution error", details: stderr || error.message });
    }

    try {
      const cleanOutput = stdout
        .trim()
        .replace(/\r/g, "")
        .replace(/\n/g, "")
        .replace(/,{}]$/, "]")
        .replace(/,\s*]$/, "]");

      const json = JSON.parse(cleanOutput);
      res.json(json);
    } catch (err) {
      console.error("⚠️ JSON Parse Error:", err.message);
      res.status(500).json({
        error: "Failed to parse C output",
        raw: stdout || "No output from analyzer",
      });
    }
  });

  // Send source code to the C process
  child.stdin.write(code);
  child.stdin.end();
});

const PORT = 5000;
app.listen(PORT, () =>
  console.log(`✅ Server running at http://localhost:${PORT}`)
);
