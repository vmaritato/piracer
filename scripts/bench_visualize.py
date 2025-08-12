#!/usr/bin/env python3
"""
Advanced benchmark visualization script for PiRacer v1.0.0
Creates interactive charts and performance analysis
"""

import argparse
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np
from pathlib import Path
import json
import sys
from typing import Dict, List, Tuple, Optional

# Set style for better looking charts
plt.style.use('seaborn-v0_8')
sns.set_palette("husl")

class BenchmarkVisualizer:
    def __init__(self, data_file: str):
        """Initialize visualizer with benchmark data."""
        self.data_file = data_file
        self.data = None
        self.load_data()
        
    def load_data(self):
        """Load benchmark data from CSV or JSON."""
        try:
            if self.data_file.endswith('.csv'):
                self.data = pd.read_csv(self.data_file)
            elif self.data_file.endswith('.json'):
                with open(self.data_file, 'r') as f:
                    self.data = pd.DataFrame(json.load(f))
            else:
                print(f"Unsupported file format: {self.data_file}")
                sys.exit(1)
                
            print(f"✅ Loaded {len(self.data)} benchmark records")
            print(f"Columns: {list(self.data.columns)}")
            
        except Exception as e:
            print(f"❌ Error loading data: {e}")
            sys.exit(1)
    
    def create_performance_scaling_chart(self, output_file: str = "performance_scaling.png"):
        """Create performance scaling chart showing ns/digit vs digits."""
        plt.figure(figsize=(12, 8))
        
        # Group by algorithm if available
        if 'algorithm' in self.data.columns:
            for algo in self.data['algorithm'].unique():
                algo_data = self.data[self.data['algorithm'] == algo]
                plt.loglog(algo_data['digits'], algo_data['ns_per_digit'], 
                          marker='o', label=algo, linewidth=2, markersize=8)
        else:
            plt.loglog(self.data['digits'], self.data['ns_per_digit'], 
                      marker='o', linewidth=2, markersize=8)
        
        plt.xlabel('Digits', fontsize=14)
        plt.ylabel('Performance (ns/digit)', fontsize=14)
        plt.title('PiRacer Performance Scaling Analysis', fontsize=16, fontweight='bold')
        plt.grid(True, alpha=0.3)
        plt.legend(fontsize=12)
        plt.tight_layout()
        
        plt.savefig(output_file, dpi=300, bbox_inches='tight')
        print(f"📊 Performance scaling chart saved to {output_file}")
        plt.close()
    
    def create_speedup_comparison_chart(self, output_file: str = "speedup_comparison.png"):
        """Create speedup comparison chart vs competitors."""
        if 'competitor' not in self.data.columns:
            print("⚠️  No competitor data found, skipping speedup chart")
            return
            
        plt.figure(figsize=(14, 8))
        
        competitors = self.data['competitor'].unique()
        digits_list = sorted(self.data['digits'].unique())
        
        x = np.arange(len(digits_list))
        width = 0.35
        
        for i, comp in enumerate(competitors):
            comp_data = self.data[self.data['competitor'] == comp]
            speedups = [comp_data[comp_data['digits'] == d]['speedup'].iloc[0] 
                       if len(comp_data[comp_data['digits'] == d]) > 0 else 0 
                       for d in digits_list]
            
            plt.bar(x + i * width, speedups, width, label=comp, alpha=0.8)
        
        plt.xlabel('Digits', fontsize=14)
        plt.ylabel('Speedup Factor', fontsize=14)
        plt.title('PiRacer vs Competitors - Speedup Analysis', fontsize=16, fontweight='bold')
        plt.xticks(x + width/2, [f"{d:,}" for d in digits_list])
        plt.legend(fontsize=12)
        plt.grid(True, alpha=0.3, axis='y')
        plt.tight_layout()
        
        plt.savefig(output_file, dpi=300, bbox_inches='tight')
        print(f"🏆 Speedup comparison chart saved to {output_file}")
        plt.close()
    
    def create_memory_analysis_chart(self, output_file: str = "memory_analysis.png"):
        """Create memory usage analysis chart."""
        if 'memory_mb' not in self.data.columns:
            print("⚠️  No memory data found, skipping memory chart")
            return
            
        plt.figure(figsize=(12, 8))
        
        # Memory vs digits
        plt.subplot(2, 1, 1)
        if 'algorithm' in self.data.columns:
            for algo in self.data['algorithm'].unique():
                algo_data = self.data[self.data['algorithm'] == algo]
                plt.loglog(algo_data['digits'], algo_data['memory_mb'], 
                          marker='s', label=algo, linewidth=2)
        else:
            plt.loglog(self.data['digits'], self.data['memory_mb'], 
                      marker='s', linewidth=2)
        
        plt.xlabel('Digits')
        plt.ylabel('Memory Usage (MB)')
        plt.title('Memory Usage Scaling')
        plt.grid(True, alpha=0.3)
        plt.legend()
        
        # Memory efficiency (ns/digit per MB)
        plt.subplot(2, 1, 2)
        if 'algorithm' in self.data.columns:
            for algo in self.data['algorithm'].unique():
                algo_data = self.data[self.data['algorithm'] == algo]
                efficiency = algo_data['ns_per_digit'] / algo_data['memory_mb']
                plt.loglog(algo_data['digits'], efficiency, 
                          marker='^', label=algo, linewidth=2)
        else:
            efficiency = self.data['ns_per_digit'] / self.data['memory_mb']
            plt.loglog(self.data['digits'], efficiency, marker='^', linewidth=2)
        
        plt.xlabel('Digits')
        plt.ylabel('Efficiency (ns/digit per MB)')
        plt.title('Memory Efficiency')
        plt.grid(True, alpha=0.3)
        plt.legend()
        
        plt.tight_layout()
        plt.savefig(output_file, dpi=300, bbox_inches='tight')
        print(f"💾 Memory analysis chart saved to {output_file}")
        plt.close()
    
    def create_algorithm_comparison_chart(self, output_file: str = "algorithm_comparison.png"):
        """Create algorithm comparison chart."""
        if 'algorithm' not in self.data.columns:
            print("⚠️  No algorithm data found, skipping algorithm chart")
            return
            
        plt.figure(figsize=(14, 10))
        
        # Performance comparison
        plt.subplot(2, 2, 1)
        algo_perf = self.data.groupby('algorithm')['ns_per_digit'].mean().sort_values()
        algo_perf.plot(kind='bar', color='skyblue', alpha=0.8)
        plt.title('Average Performance by Algorithm')
        plt.ylabel('ns/digit')
        plt.xticks(rotation=45)
        plt.grid(True, alpha=0.3, axis='y')
        
        # Memory comparison
        if 'memory_mb' in self.data.columns:
            plt.subplot(2, 2, 2)
            algo_mem = self.data.groupby('algorithm')['memory_mb'].mean().sort_values()
            algo_mem.plot(kind='bar', color='lightcoral', alpha=0.8)
            plt.title('Average Memory Usage by Algorithm')
            plt.ylabel('Memory (MB)')
            plt.xticks(rotation=45)
            plt.grid(True, alpha=0.3, axis='y')
        
        # Scaling comparison
        plt.subplot(2, 2, 3)
        for algo in self.data['algorithm'].unique():
            algo_data = self.data[self.data['algorithm'] == algo]
            plt.loglog(algo_data['digits'], algo_data['ns_per_digit'], 
                      marker='o', label=algo, linewidth=2)
        plt.xlabel('Digits')
        plt.ylabel('ns/digit')
        plt.title('Performance Scaling by Algorithm')
        plt.grid(True, alpha=0.3)
        plt.legend()
        
        # Distribution analysis
        plt.subplot(2, 2, 4)
        for algo in self.data['algorithm'].unique():
            algo_data = self.data[self.data['algorithm'] == algo]
            plt.hist(algo_data['ns_per_digit'], alpha=0.6, label=algo, bins=20)
        plt.xlabel('ns/digit')
        plt.ylabel('Frequency')
        plt.title('Performance Distribution by Algorithm')
        plt.legend()
        plt.grid(True, alpha=0.3)
        
        plt.tight_layout()
        plt.savefig(output_file, dpi=300, bbox_inches='tight')
        print(f"🔬 Algorithm comparison chart saved to {output_file}")
        plt.close()
    
    def create_interactive_dashboard(self, output_file: str = "benchmark_dashboard.html"):
        """Create interactive HTML dashboard using plotly."""
        try:
            import plotly.graph_objects as go
            import plotly.express as px
            from plotly.subplots import make_subplots
            import plotly.offline as pyo
        except ImportError:
            print("⚠️  Plotly not available, skipping interactive dashboard")
            return
        
        # Create subplots
        fig = make_subplots(
            rows=2, cols=2,
            subplot_titles=('Performance Scaling', 'Speedup vs Competitors', 
                          'Memory Usage', 'Algorithm Comparison'),
            specs=[[{"type": "scatter"}, {"type": "bar"}],
                   [{"type": "scatter"}, {"type": "bar"}]]
        )
        
        # Performance scaling
        if 'algorithm' in self.data.columns:
            for algo in self.data['algorithm'].unique():
                algo_data = self.data[self.data['algorithm'] == algo]
                fig.add_trace(
                    go.Scatter(x=algo_data['digits'], y=algo_data['ns_per_digit'],
                              mode='lines+markers', name=algo, log_x=True, log_y=True),
                    row=1, col=1
                )
        else:
            fig.add_trace(
                go.Scatter(x=self.data['digits'], y=self.data['ns_per_digit'],
                          mode='lines+markers', name='Performance', log_x=True, log_y=True),
                row=1, col=1
            )
        
        # Speedup comparison
        if 'competitor' in self.data.columns:
            competitors = self.data['competitor'].unique()
            for comp in competitors:
                comp_data = self.data[self.data['competitor'] == comp]
                fig.add_trace(
                    go.Bar(x=comp_data['digits'], y=comp_data['speedup'],
                           name=comp, opacity=0.8),
                    row=1, col=2
                )
        
        # Memory usage
        if 'memory_mb' in self.data.columns:
            if 'algorithm' in self.data.columns:
                for algo in self.data['algorithm'].unique():
                    algo_data = self.data[self.data['algorithm'] == algo]
                    fig.add_trace(
                        go.Scatter(x=algo_data['digits'], y=algo_data['memory_mb'],
                                  mode='lines+markers', name=f"{algo} Memory", log_x=True, log_y=True),
                        row=2, col=1
                    )
            else:
                fig.add_trace(
                    go.Scatter(x=self.data['digits'], y=self.data['memory_mb'],
                              mode='lines+markers', name='Memory Usage', log_x=True, log_y=True),
                    row=2, col=1
                )
        
        # Algorithm comparison
        if 'algorithm' in self.data.columns:
            algo_perf = self.data.groupby('algorithm')['ns_per_digit'].mean().sort_values()
            fig.add_trace(
                go.Bar(x=list(algo_perf.index), y=list(algo_perf.values),
                       name='Avg Performance', marker_color='lightblue'),
                row=2, col=2
            )
        
        # Update layout
        fig.update_layout(
            title_text="PiRacer Benchmark Dashboard",
            title_x=0.5,
            height=800,
            showlegend=True
        )
        
        # Update axes
        fig.update_xaxes(title_text="Digits", row=1, col=1)
        fig.update_yaxes(title_text="ns/digit", row=1, col=1)
        fig.update_xaxes(title_text="Digits", row=1, col=2)
        fig.update_yaxes(title_text="Speedup", row=1, col=2)
        fig.update_xaxes(title_text="Digits", row=2, col=1)
        fig.update_yaxes(title_text="Memory (MB)", row=2, col=1)
        fig.update_xaxes(title_text="Algorithm", row=2, col=2)
        fig.update_yaxes(title_text="ns/digit", row=2, col=2)
        
        # Save interactive dashboard
        pyo.plot(fig, filename=output_file, auto_open=False)
        print(f"🎯 Interactive dashboard saved to {output_file}")
    
    def generate_performance_report(self, output_file: str = "performance_report.txt"):
        """Generate comprehensive performance report."""
        report = []
        report.append("=" * 60)
        report.append("PIRACER PERFORMANCE ANALYSIS REPORT")
        report.append("=" * 60)
        report.append(f"Generated from: {self.data_file}")
        report.append(f"Total records: {len(self.data)}")
        report.append(f"Date range: {self.data['timestamp'].min()} to {self.data['timestamp'].max()}")
        report.append("")
        
        # Performance summary
        report.append("PERFORMANCE SUMMARY")
        report.append("-" * 30)
        report.append(f"Best performance: {self.data['ns_per_digit'].min():.2f} ns/digit")
        report.append(f"Worst performance: {self.data['ns_per_digit'].max():.2f} ns/digit")
        report.append(f"Average performance: {self.data['ns_per_digit'].mean():.2f} ns/digit")
        report.append(f"Performance std dev: {self.data['ns_per_digit'].std():.2f} ns/digit")
        report.append("")
        
        # Algorithm analysis
        if 'algorithm' in self.data.columns:
            report.append("ALGORITHM ANALYSIS")
            report.append("-" * 30)
            algo_stats = self.data.groupby('algorithm').agg({
                'ns_per_digit': ['mean', 'std', 'min', 'max'],
                'digits': ['count', 'mean']
            }).round(2)
            report.append(str(algo_stats))
            report.append("")
        
        # Scaling analysis
        report.append("SCALING ANALYSIS")
        report.append("-" * 30)
        digit_groups = self.data.groupby('digits')['ns_per_digit'].agg(['mean', 'std', 'count'])
        for digits, stats in digit_groups.iterrows():
            report.append(f"{digits:,} digits: {stats['mean']:.2f} ± {stats['std']:.2f} ns/digit (n={stats['count']})")
        report.append("")
        
        # Recommendations
        report.append("RECOMMENDATIONS")
        report.append("-" * 30)
        best_algo = self.data.loc[self.data['ns_per_digit'].idxmin()]
        report.append(f"Best algorithm: {best_algo.get('algorithm', 'Unknown')}")
        report.append(f"Optimal digit range: {best_algo['digits']:,} digits")
        report.append(f"Expected performance: {best_algo['ns_per_digit']:.2f} ns/digit")
        
        # Save report
        with open(output_file, 'w') as f:
            f.write('\n'.join(report))
        
        print(f"📋 Performance report saved to {output_file}")
        return '\n'.join(report)
    
    def run_all_visualizations(self, output_dir: str = "benchmark_charts"):
        """Run all visualization methods."""
        Path(output_dir).mkdir(exist_ok=True)
        
        print(f"🚀 Generating visualizations in {output_dir}/")
        
        # Generate all charts
        self.create_performance_scaling_chart(f"{output_dir}/performance_scaling.png")
        self.create_speedup_comparison_chart(f"{output_dir}/speedup_comparison.png")
        self.create_memory_analysis_chart(f"{output_dir}/memory_analysis.png")
        self.create_algorithm_comparison_chart(f"{output_dir}/algorithm_comparison.png")
        
        # Generate interactive dashboard
        self.create_interactive_dashboard(f"{output_dir}/benchmark_dashboard.html")
        
        # Generate performance report
        self.generate_performance_report(f"{output_dir}/performance_report.txt")
        
        print(f"✅ All visualizations completed in {output_dir}/")

def main():
    parser = argparse.ArgumentParser(description="PiRacer Benchmark Visualizer v1.0.0")
    parser.add_argument("data_file", help="Benchmark data file (CSV or JSON)")
    parser.add_argument("--output-dir", default="benchmark_charts", 
                       help="Output directory for charts (default: benchmark_charts)")
    parser.add_argument("--chart", choices=['scaling', 'speedup', 'memory', 'algorithm', 'all'],
                       default='all', help="Specific chart to generate (default: all)")
    
    args = parser.parse_args()
    
    if not Path(args.data_file).exists():
        print(f"❌ Data file not found: {args.data_file}")
        sys.exit(1)
    
    # Create visualizer
    viz = BenchmarkVisualizer(args.data_file)
    
    # Generate requested charts
    if args.chart == 'all':
        viz.run_all_visualizations(args.output_dir)
    else:
        Path(args.output_dir).mkdir(exist_ok=True)
        
        if args.chart == 'scaling':
            viz.create_performance_scaling_chart(f"{args.output_dir}/performance_scaling.png")
        elif args.chart == 'speedup':
            viz.create_speedup_comparison_chart(f"{args.output_dir}/speedup_comparison.png")
        elif args.chart == 'memory':
            viz.create_memory_analysis_chart(f"{args.output_dir}/memory_analysis.png")
        elif args.chart == 'algorithm':
            viz.create_algorithm_comparison_chart(f"{args.output_dir}/algorithm_comparison.png")
    
    print("🎉 Visualization complete!")

if __name__ == "__main__":
    main() 