import React from "react";


export default function ErrorList({ errors }) {
  if (!errors || errors.length === 0) {
    return <div className="no-error">✅ No errors found!</div>;
  }

  // handle case where backend accidentally sends a string
  const errorList = typeof errors === "string" ? JSON.parse(errors) : errors;

  return (
    <div className="error-list">
      {errorList.map((err, idx) => (
        <div key={idx} className={`error-item ${err.type?.toLowerCase() || "server"}`}>
          <strong>
            {err.type || "Server"} (line {err.line || 0}):
          </strong>{" "}
          {err.message}
        </div>
      ))}
    </div>
  );
}
